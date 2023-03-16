#include "Object.h"

Object::Object()
{
}

Object::~Object()
{
}

bool Object::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	BuildConstantBuffers(pd3dDevice);

	// 제거 예정

	m_pMesh = std::make_shared<Mesh>();
	if (m_pMesh == nullptr)
		return false;
	else 
		m_pMesh->BuildMesh(pd3dDevice, pd3dCommandList);

	m_ppMaterials.emplace_back(std::make_shared<Material>());

	for (int i = 0; i < m_ppMaterials.size(); ++i)
	{
		if (m_ppMaterials[i] == nullptr)
			return false;
		else
		{
			m_ppMaterials[i]->LoadTexture(pd3dDevice, pd3dCommandList, L"Textures\\bricks.dds");
			m_ppMaterials[i]->BuildDescriptorHeap(pd3dDevice);
		}
	}
	// --------------------

	return true;
}

void Object::Update(const GameTimer& gt)
{
	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
	
	XMMATRIX world = XMMatrixMultiply(XMMatrixRotationY(m_Yaw), XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), m_Pitch));
	world = XMMatrixMultiply(world, XMLoadFloat4x4(&m_xmf4x4World));

	tmpObjConstant objConstant;
	XMStoreFloat4x4(&objConstant.World, XMMatrixTranspose(world));
	if(m_pObjectCB) m_pObjectCB->CopyData(0, objConstant);

	if (m_pSibling) m_pSibling->Update(gt);
	if (m_pChild) {
		m_pChild->SetParentWorld(m_xmf4x4World);
		m_pChild->Update(gt);
	}
}

void Object::PrepareRender(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pObjectCB) pd3dCommandList->SetGraphicsRootConstantBufferView(0, m_pObjectCB->Resource()->GetGPUVirtualAddress());
}

void Object::Render(const GameTimer& gt, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_ppMaterials.size(); ++i)
	{
		m_ppMaterials[i]->OnPrepareRender(pd3dCommandList);
	}

	if (m_pMesh)
	{
		m_pMesh->OnprepareRender(gt, pd3dCommandList);
		m_pMesh->Render(gt, pd3dCommandList);
	}

	if (m_pSibling) m_pSibling->Render(gt, pd3dCommandList);
	if (m_pChild) m_pChild->Render(gt, pd3dCommandList);
}

void Object::BuildConstantBuffers(ID3D12Device* pd3dDevice)
{
	m_pObjectCB = std::make_unique<UploadBuffer<tmpObjConstant>>(pd3dDevice, 1, true);
	m_ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(tmpObjConstant));
}



std::shared_ptr<ModelDataInfo> Object::LoadModelDataFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName)
{
	return nullptr;
}

std::shared_ptr<Object> Object::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	std::shared_ptr<Object> pObject;

	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nFrame = ReadintegerFromFile(pInFile);
			nTextures = ReadintegerFromFile(pInFile);

			ReadStringFromFile(pInFile, pObject->m_FrameName);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 position, rotation, scale;
			XMFLOAT4 quaternion;

			nReads = (UINT)fread(&position, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&rotation, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&scale, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&quaternion, sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)fread(&pObject->m_xmf4x4LocalTransform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			pObject->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			// =======================================
			// =======================================
			// =======================================
			// ============== 미 구 현 ===============
			// ============== 미 구 현 ===============
			// =======================================
			// =======================================
			// =======================================
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChild = ReadintegerFromFile(pInFile);

			if (nChild > 0)
			{
				for (int i = 0; i < nChild; ++i)
				{
					std::shared_ptr<Object> pChild = LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pInFile);
					if (pChild) pObject->SetChild(pChild);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif

				}
			}

		}
		else if (!strcmp(pstrToken, "</Frame>:"))
		{
			break;
		}
	}

	return pObject;
}

void Object::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommnadList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	
	UINT nReads;

	std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>();

	int nVertices = 0;

	nVertices = ReadintegerFromFile(pInFile);
	nReads = ReadStringFromFile(pInFile, pstrToken);
	pMesh->SetMeshName(pstrToken);

	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;
	vertices.resize(nVertices);
	indices.resize(nVertices);

	for (; ; )
	{
		nReads = ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:"))
		{

		}
		else if (!strcmp(pstrToken, "<Position>:"))
		{

		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{

		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{

		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{

		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{

		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{

		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{

		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{

		}
		else if (!strcmp(pstrToken, "</Mesh>:"))
		{
			break;
		}
	}

}

void Object::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;

	UINT nReads;

	std::shared_ptr<Material> pMat = std::make_shared<Material>();

	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);
			break;
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			XMFLOAT4 tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 4, pInFile);
			pMat->SetAlbedoColor(tmp);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			XMFLOAT4 tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 4, pInFile);
			pMat->SetEmissiveColor(tmp);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			XMFLOAT4 tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 4, pInFile);
			pMat->SetSpecularColor(tmp);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			pMat->SetGlossiness(tmp);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			pMat->SetSmoothness(tmp);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			pMat->SetMetallic(tmp);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			pMat->SetMetallic(tmp);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			pMat->SetGlossyReflection(tmp);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			pMat->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			pMat->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			pMat->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			pMat->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			pMat->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			pMat->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			pMat->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}

	SetMaterial(pMat);
}

void Object::LoadAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
}



void Object::SetChild(std::shared_ptr<Object> pChild)
{
	if (m_pChild != NULL)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void Object::SetMesh(std::shared_ptr<Mesh> pMesh)
{
	m_pMesh = pMesh;
}

void Object::SetMaterial(std::shared_ptr<Material> pMaterial)
{
	m_ppMaterials.emplace_back(pMaterial);
}
