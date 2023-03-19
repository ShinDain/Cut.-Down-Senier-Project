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

	return true;
}

void Object::Update(const GameTimer& gt)
{
	m_xmf4x4World = m_xmf4x4ParentWorld;

	XMMATRIX rotate = XMMatrixMultiply(XMMatrixRotationY(m_Yaw), XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), m_Pitch));
	XMMATRIX world = XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4LocalTransform), XMLoadFloat4x4(&m_xmf4x4World));

	XMMATRIX lastMat = XMMatrixMultiply(rotate, world);

	XMStoreFloat4x4(&m_xmf4x4World, lastMat);
	
	tmpObjConstant objConstant;
	XMStoreFloat4x4(&objConstant.World, XMMatrixTranspose(lastMat));
	if(m_pObjectCB) m_pObjectCB->CopyData(0, objConstant);

	if (m_pSibling) {
		m_pSibling->SetParentWorld(m_xmf4x4ParentWorld);
		m_pSibling->Update(gt);
	}
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
	PrepareRender(gt, pd3dCommandList);

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

std::shared_ptr<Object> Object::LoadModelDataFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	std::shared_ptr<Object> pObject;

	pObject = Object::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pInFile);
	
	return pObject;
}

std::shared_ptr<Object> Object::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	std::shared_ptr<Object> pObject = std::make_shared<Object>();
	pObject->BuildConstantBuffers(pd3dDevice);

	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nFrame = ReadintegerFromFile(pInFile);
			nTextures = ReadintegerFromFile(pInFile);

			char tmpstr[64] = { '\0' };
			ReadStringFromFile(pInFile, tmpstr);
			
			strcpy_s(pObject->m_FrameName, tmpstr);
			pObject->SetName(tmpstr);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			nReads = (UINT)fread(&pObject->m_xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&pObject->m_Pitch, sizeof(float), 1, pInFile);
			nReads = (UINT)fread(&pObject->m_Yaw, sizeof(float), 1, pInFile);
			nReads = (UINT)fread(&pObject->m_Roll, sizeof(float), 1, pInFile);
			nReads = (UINT)fread(&pObject->m_xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)fread(&pObject->m_xmf4Quaternion, sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)fread(&pObject->m_xmf4x4LocalTransform, sizeof(XMFLOAT4X4), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>();
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pObject->SetMesh(pMesh);
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
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}

	return pObject;
}

void Object::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	int nMatcnt = 0;

	UINT nReads;
	nMaterial = ReadintegerFromFile(pInFile);

	std::vector<std::shared_ptr<Material>> ppMat;

	for (int i = 0; i < nMaterial; ++i)
	{
		ppMat.emplace_back(std::make_shared<Material>());
	}

	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMatcnt = ReadintegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			XMFLOAT4 tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 4, pInFile);
			ppMat[nMatcnt]->SetAlbedoColor(tmp);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			XMFLOAT4 tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 4, pInFile);
			ppMat[nMatcnt]->SetEmissiveColor(tmp);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			XMFLOAT4 tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 4, pInFile);
			ppMat[nMatcnt]->SetSpecularColor(tmp);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			ppMat[nMatcnt]->SetGlossiness(tmp);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			ppMat[nMatcnt]->SetSmoothness(tmp);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			ppMat[nMatcnt]->SetMetallic(tmp);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			ppMat[nMatcnt]->SetMetallic(tmp);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			float tmp;
			nReads = (UINT)::fread(&tmp, sizeof(float), 1, pInFile);
			ppMat[nMatcnt]->SetGlossyReflection(tmp);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			ppMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			ppMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			ppMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			ppMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			ppMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			ppMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			ppMat[nMatcnt]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pInFile);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}

	for (int i = 0; i < ppMat.size(); ++i)
		ppMat[i]->BuildDescriptorHeap(pd3dDevice);

	SetMaterials(ppMat);
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

void Object::SetMaterials(std::vector<std::shared_ptr<Material>> ppMaterial)
{
	m_ppMaterials = ppMaterial;
}
