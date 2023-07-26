#include "../Header/BackfaceObject.h"

BackfaceObject::BackfaceObject()
{
}

BackfaceObject::BackfaceObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
	m_nObjectCBParameterIdx = 3;

	m_DestroyTime = 0.0;
	m_DissolveTime = 0.0f;
}

BackfaceObject::~BackfaceObject()
{
	Object::Destroy();
	m_pOriginalObject.reset();
}

bool BackfaceObject::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	std::shared_ptr<ModelDataInfo> pModelData = pModel;

	if (pModelData)
	{
		SetChild(pModelData->m_pRootObject);
	}

	m_nObjectType = ObjectType::Object_None;
	m_nColliderType = ColliderType::Collider_None;
	m_xmf3Scale = objData.xmf3Scale;

	//m_xmf3ColliderExtents = objData.xmf3Extents;
	m_xmf3RenderOffsetPosition = objData.xmf3MeshOffsetPosition;
	m_xmf3RenderOffsetRotation = objData.xmf3MeshOffsetRotation;
	m_bShadow = objData.bShadow;

	return true;
}

void BackfaceObject::Update(float elapsedTime)
{
	if (!m_bIsAlive)
		return;

	// 오브젝트 파괴 타이머
	if (m_bDestroying)
	{
		if (m_bDissolveStart)
		{
			m_ElapsedDestroyTime += elapsedTime;
			m_DissolveValue = m_ElapsedDestroyTime / (m_DestroyTime);
			if (m_ElapsedDestroyTime >= m_DestroyTime)
			{
				m_bIsAlive = false;
				return;
			}
		}
		else
		{
			m_ElapsedDissolveTime += elapsedTime;
			if (m_ElapsedDissolveTime >= m_DissolveTime)
			{
				m_bDissolveStart = true;
			}
		}
	}

	if (m_pOriginalObject)
	{
		if (m_pOriginalObject->GetPicked())
			m_bVisible = true;
		else
			m_bVisible = false;	

		m_xmf3Position = m_pOriginalObject->GetPosition();
		m_xmf3Rotation = m_pOriginalObject->GetRotation();
		m_xmf4Orientation = m_pOriginalObject->GetOrientation();
		if(!m_pOriginalObject->GetIsAlive() || m_pOriginalObject->GetIsDestroying())
			m_bIsAlive = false;
	}
	else
	{
		m_bIsAlive = false;
	}
	
	if (m_pSibling) {
		m_pSibling->Update(elapsedTime);
	}
	if (m_pChild) {
		m_pChild->Update(elapsedTime);
	}
}

void BackfaceObject::DestroyRunTime()
{
	Object::DestroyRunTime();
	if(m_pOriginalObject)
		m_pOriginalObject.reset();
}

//void BackfaceObject::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	if (!m_bVisible)
//		return;
//
//	OnPrepareRender(elapsedTime, pd3dCommandList);
//
//	if (m_pMesh)
//	{
//		XMFLOAT4X4 xmf4x4World;
//		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
//		pd3dCommandList->SetGraphicsRoot32BitConstants(m_nObjectConstantsParameterIdx, 16, &xmf4x4World, 0);
//		XMFLOAT4X4 xmf4x4InverseTransWorld;
//		XMStoreFloat4x4(&xmf4x4InverseTransWorld, XMMatrixTranspose(XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_xmf4x4World)))));
//		pd3dCommandList->SetGraphicsRoot32BitConstants(m_nObjectConstantsParameterIdx, 16, &xmf4x4InverseTransWorld, 16);
//
//		for (int i = 0; i < m_vpMaterials.size(); ++i)
//		{
//			m_vpMaterials[i]->MaterialSet(pd3dCommandList);
//
//			m_pMesh->Render(elapsedTime, pd3dCommandList, i);
//		}
//	}
//
//#if defined(_DEBUG) && defined(COLLIDER_RENDER)
//	if (m_pCollider)
//	{
//		g_Shaders[ShaderType::Shader_WireFrame]->ChangeShader(pd3dCommandList);
//		m_pCollider->Render(elapsedTime, pd3dCommandList);
//	}
//#endif
//
//	if (m_pSibling) m_pSibling->Render(elapsedTime, pd3dCommandList);
//	if (m_pChild) m_pChild->Render(elapsedTime, pd3dCommandList);
//
//}
