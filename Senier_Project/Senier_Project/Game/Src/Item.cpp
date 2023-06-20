#include "../Header/Item.h"

Item::Item()
{
}

Item::Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

Item::~Item()
{
	Destroy();
}

bool Item::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	std::shared_ptr<ModelDataInfo> pModelData = pModel;

	SetChild(pModelData->m_pRootObject);
	if (nAnimationTracks > 0)
		m_pAnimationController = std::make_unique<AnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pModelData);

	m_pBody = nullptr;
	m_pCollider = nullptr;
	m_xmf3RenderOffsetPosition = objData.xmf3MeshOffsetPosition;
	m_xmf3RenderOffsetRotation = objData.xmf3MeshOffsetRotation;

	m_xmf3Position = objData.xmf3Position;
	m_xmf3Rotation = objData.xmf3Rotation;
	m_xmf3Scale = objData.xmf3Scale;
	m_xmf4Orientation = objData.xmf4Orientation;
	m_Mass = objData.nMass;

#if defined(_DEBUG)
	if (m_pCollider) m_pCollider->BuildMesh(pd3dDevice, pd3dCommandList);
#endif
	BuildConstantBuffers(pd3dDevice);
	m_ItemCollider.Center = m_xmf3Position;
	m_ItemCollider.Radius = m_ItemColliderRadius;

	m_TraceCollider.Center = m_xmf3Position;
	m_TraceCollider.Radius = m_TraceColliderRadius;

	m_bShadow = objData.bShadow;

	return true;
}

void Item::Animate(float elapsedTime)
{
	m_xmf3Rotation.y += elapsedTime * 500;
}

void Item::Update(float elapsedTime)
{
	if (!m_bIsAlive)
		return;

	if (m_bIsActive)
		Intersect(elapsedTime);
	else
	{
		m_ElapsedActiveTime += elapsedTime;
		if (m_ElapsedActiveTime > m_ActiveTime)
			m_bIsActive = true;		
	}

	Animate(elapsedTime);
	UpdateTransform(NULL);

	if (m_pObjectCB) UpdateObjectCB();

	if (m_pSibling) {
		m_pSibling->Update(elapsedTime);
	}
	if (m_pChild) {
		m_pChild->Update(elapsedTime);
	}
}

void Item::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	if (pxmf4x4Parent)
	{
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4LocalTransform), XMLoadFloat4x4(pxmf4x4Parent)));
	}
	else
	{
		// RootObjectÀÎ °æ¿ì
		m_xmf4x4LocalTransform = MathHelper::identity4x4();
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX xmmatScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
		XMMATRIX xmmatRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotation.x), XMConvertToRadians(m_xmf3Rotation.y), XMConvertToRadians(m_xmf3Rotation.z));
		XMMATRIX xmmatTranslate = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
		// S * R * T
		world = XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatRotate, xmmatTranslate));

		XMMATRIX offset = XMMatrixTranslation(-m_xmf3RenderOffsetPosition.x, -m_xmf3RenderOffsetPosition.y, -m_xmf3RenderOffsetPosition.z);
		XMMATRIX offsetRotate = XMMatrixRotationRollPitchYaw(m_xmf3RenderOffsetRotation.x, m_xmf3RenderOffsetRotation.y, m_xmf3RenderOffsetRotation.z);
		world = XMMatrixMultiply(XMMatrixMultiply(offsetRotate, offset), world);
		XMStoreFloat4x4(&m_xmf4x4LocalTransform, world);

		m_xmf4x4World = m_xmf4x4LocalTransform;

		m_xmf3RenderPosition = XMFLOAT3(0, 0, 0);
		XMVECTOR renderPosition = XMLoadFloat3(&m_xmf3RenderPosition);
		renderPosition = XMVector3TransformCoord(renderPosition, world);
		XMStoreFloat3(&m_xmf3RenderPosition, renderPosition);
	}

	if (m_pSibling) {
		m_pSibling->UpdateTransform(pxmf4x4Parent);
	}
	if (m_pChild) {
		m_pChild->UpdateTransform(&m_xmf4x4World);
	}
}

void Item::Render(float elapsedTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Object::Render(elapsedTime, pd3dCommandList);
}

void Item::Intersect(float elapsedTime)
{
	BoundingOrientedBox playerCollider = std::static_pointer_cast<ColliderBox>(g_pPlayer->GetCollider())->GetOBB();

	if (m_bTrace)
	{
		TracePlayer(elapsedTime);

		if (m_ItemCollider.Intersects(playerCollider))
		{
			std::shared_ptr<Player> pPlayer = std::static_pointer_cast<Player>(g_pPlayer);
			pPlayer->AcquireItem(m_nItemType);

			m_bIsAlive = false;
			return;
		}
	}
	else
	{
		if (m_TraceCollider.Intersects(playerCollider))
		{
			m_bTrace = true;
			return;
		}
	}
}

void Item::TracePlayer(float elapsedTime)
{
	XMVECTOR playerPos = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR thisPos = XMLoadFloat3(&m_xmf3Position);

	XMVECTOR direction = playerPos - thisPos;
	direction = XMVector3Normalize(direction);
	XMVECTOR newPosition = thisPos + direction * m_PlayerTraceSpeed * elapsedTime;
	XMStoreFloat3(&m_xmf3Position, newPosition);

	m_ItemCollider.Center = m_xmf3Position;
}

void Item::Destroy()
{
	Object::Destroy();
}
