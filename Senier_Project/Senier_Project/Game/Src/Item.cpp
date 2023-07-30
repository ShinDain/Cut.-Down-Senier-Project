#include "../Header/Item.h"
#include "../../DirectXRendering/Header/Scene.h"

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

	m_pBody = std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation, objData.xmf3Rotation, objData.xmf3Scale, objData.nMass);

	XMFLOAT3 xmf3RandVelocity = {0,0,0};
	xmf3RandVelocity.x = rand() % 200 - 100;
	xmf3RandVelocity.y = rand() % 30 + 5;
	xmf3RandVelocity.z = rand() % 200 - 100;
	m_pBody->SetVelocity(xmf3RandVelocity);

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
	m_IntersectCollider.Center = m_xmf3Position;
	m_IntersectCollider.Radius = m_IntersectColliderRadius;

	m_TraceCollider.Center = m_xmf3Position;
	m_TraceCollider.Radius = m_TraceColliderRadius;

	m_bShadow = objData.bShadow;

	UpdateTransform(nullptr);

	// 시작 속도 결정
	//CalcStartVelocity();

	return true;
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
		{
			m_bIsActive = true;
		}
	}

	m_pBody->SetAngularVelocity(XMFLOAT3(0, 5, 0));
	UpdateTransform(NULL);

	UpdateToRigidBody(elapsedTime);

	if (m_xmf3Position.y < 10 && !m_bTrace)
	{
		m_pBody->SetInGravity(false);
		m_xmf3Position.y = 10;
		XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
		xmf3Velocity.x *= 0.9f;
		xmf3Velocity.y = 0;
		xmf3Velocity.z *= 0.9f;
		if (xmf3Velocity.x < 1)
			xmf3Velocity.x = 0;
		if (xmf3Velocity.z < 1)
			xmf3Velocity.z = 0;
		m_pBody->SetVelocity(xmf3Velocity);
	}
	else
	{
		XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
		xmf3Velocity.x *= 0.9f;
		xmf3Velocity.z *= 0.9f;
		if (xmf3Velocity.x < 1)
			xmf3Velocity.x = 0;
		if (xmf3Velocity.z < 1)
			xmf3Velocity.z = 0;
		m_pBody->SetVelocity(xmf3Velocity);
	}

	
	m_IntersectCollider.Center = m_xmf3Position;
	m_TraceCollider.Center = m_xmf3Position;

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
		// RootObject인 경우
		m_xmf4x4LocalTransform = MathHelper::identity4x4();
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX xmmatScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
		XMMATRIX xmmatOrientation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));
		XMMATRIX xmmatRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotation.x), XMConvertToRadians(m_xmf3Rotation.y), XMConvertToRadians(m_xmf3Rotation.z));
		XMMATRIX xmmatTranslate = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
		// S * R * T
		xmmatRotate = XMMatrixMultiply(xmmatRotate, xmmatOrientation);
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

void Item::Intersect(float elapsedTime)
{
	BoundingOrientedBox* playerCollider = std::static_pointer_cast<ColliderBox>(g_pPlayer->GetCollider())->GetOBB().get();

	if (m_bTrace)
	{
		TracePlayer(elapsedTime);

		// 플레이어와 충돌
		if (m_IntersectCollider.Intersects(*playerCollider))
		{
			std::shared_ptr<Player> pPlayer = std::static_pointer_cast<Player>(g_pPlayer);
			pPlayer->AcquireItem(m_nItemType);

			m_bIsAlive = false;
			return;
		}
	}
	else
	{
		if (m_TraceCollider.Intersects(*playerCollider))
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
	XMVECTOR velocity = direction * m_Speed;
	XMFLOAT3 xmf3Velocity;
	XMStoreFloat3(&xmf3Velocity, velocity);
	m_pBody->SetIsAwake(true);
	m_pBody->SetVelocity(xmf3Velocity);
}

void Item::CalcStartVelocity()
{
	// 월드 오브젝트와의 검사 (움직이는, 고정된)
	if (!m_bCalced)
	{

		for (int i = 0; i < g_vpWorldObjs.size(); ++i)
		{
			if (g_vpWorldObjs[i]->GetObjectType() == ObjectType::Object_Movable) continue;
			if (g_vpWorldObjs[i].get() == this || g_vpWorldObjs[i]->GetColliderType() != Collider_Box)
				continue;

			ColliderBox* objCollider = std::static_pointer_cast<ColliderBox>(g_vpWorldObjs[i]->GetCollider()).get();

			if (!objCollider->GetIsActive())
				continue;

			if (m_IntersectCollider.Intersects(*objCollider->GetOBB()))
			{
				XMVECTOR curVelocity = XMLoadFloat3(&m_pBody->GetVelocity());
				XMVECTOR crushVel = XMLoadFloat3(&m_xmf3CrushVelocity);
				crushVel = XMVector3Normalize(crushVel) * -1;
				curVelocity = XMVector3Normalize(curVelocity);
				if (XMVectorGetX(XMVector3Dot(curVelocity, crushVel)) < 0)
					curVelocity *= -1;

				curVelocity *= 50;
				XMFLOAT3 xmf3RandVelocity;
				XMStoreFloat3(&xmf3RandVelocity, curVelocity);

				XMVECTOR newPosition = XMLoadFloat3(&m_xmf3Position);
				newPosition += crushVel * 5;
				XMFLOAT3 xmf3NewPosition;
				XMStoreFloat3(&xmf3NewPosition, newPosition);

				m_pBody->SetPosition(xmf3NewPosition);
				m_pBody->SetVelocity(xmf3RandVelocity);
				m_bCalced = true;
			}
		}
	}
}
