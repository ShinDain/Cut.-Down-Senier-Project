#include "../Header/Event.h"

CEvent::CEvent()
{
}

CEvent::CEvent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

CEvent::~CEvent()
{
	Destroy();
}

bool CEvent::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	std::shared_ptr<ModelDataInfo> pModelData = pModel;

	if (pModelData)
	{
		SetChild(pModelData->m_pRootObject);
		if (nAnimationTracks > 0)
			m_pAnimationController = std::make_unique<AnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pModelData);
	}
	m_pBody = nullptr;
	m_pCollider = nullptr;
	m_nObjectType = objData.objectType;

	m_xmf3Position = objData.xmf3Position;
	m_xmf4Orientation = objData.xmf4Orientation;
	m_xmf3Rotation = objData.xmf3Rotation;
	m_xmf3Scale = objData.xmf3Scale;

	m_xmf3ColliderExtents = objData.xmf3Extents;
	m_xmf3RenderOffsetPosition = objData.xmf3MeshOffsetPosition;
	m_xmf3RenderOffsetRotation = objData.xmf3MeshOffsetRotation;
	m_bShadow = objData.bShadow;

	m_pOBB = std::make_shared<BoundingOrientedBox>();
	m_pOBB->Center = m_xmf3Position;
	m_pOBB->Extents = m_xmf3ColliderExtents;

	UpdateTransform(nullptr);

	return true;
}

void CEvent::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
#if defined(DEBUG) || (_DEBUG)

	Object::Render(elapsedTime, pd3dCommandList);

#endif

}

void CEvent::Update(float elapsedTime)
{
	// 플레이어와 교차 검사
	Intersect();
}

void CEvent::Intersect()
{
	if (!m_bIsActive)
		return;
	if (!m_bIntersect)
	{
		if (m_pOBB->Intersects(*(g_pPlayer->GetCollider()->GetBoundingSphere().get())))
		{
			m_bIntersect = true;
		}
	}
}
