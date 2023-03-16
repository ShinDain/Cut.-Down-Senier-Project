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

	m_pMaterial = std::make_shared<Material>();
	if (m_pMaterial == nullptr)
		return false;
	else
	{
		m_pMaterial->LoadTexture(pd3dDevice, pd3dCommandList, L"Textures\\bricks.dds");
		m_pMaterial->BuildDescriptorHeap(pd3dDevice);
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
	if(m_pMaterial) m_pMaterial->OnPrepareRender(pd3dCommandList);

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
			//nReads = (UINT)fread(&pObject->)
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{

		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{

		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{

		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{

		}
		else if (!strcmp(pstrToken, "</Frame>:"))
		{
			break;
		}
	}


	return pObject;
}

void Object::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommnadList, FILE* pInFile)
{
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
	m_pMaterial = pMaterial;
}
