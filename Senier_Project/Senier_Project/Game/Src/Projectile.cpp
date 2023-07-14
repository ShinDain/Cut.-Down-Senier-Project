#include "../Header/Projectile.h"

/////////////////////////////////////////////////
// 충돌체가 있는 투사체를 사용할 경우 
// 낙하 물체 충돌이 겹쳐서 발생, 유의할 것
/////////////////////////////////////////////////

Projectile::Projectile()
{
}

Projectile::Projectile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
	
	//m_bDestroying = true;
	m_DissolveTime = 3.0f;
	m_DestroyTime = 1.5f;
}

Projectile::~Projectile()
{
	Destroy();
}

bool Projectile::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	std::shared_ptr<ModelDataInfo> pModelData = pModel;

	SetChild(pModelData->m_pRootObject);
	if (nAnimationTracks > 0)
		m_pAnimationController = std::make_unique<AnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pModelData);

	std::shared_ptr<Collider> pCollider = nullptr;
	std::shared_ptr<RigidBody> pBody = nullptr;

	m_nObjectType = objData.objectType;
	// 충돌체 타입에 따라 
	m_nColliderType = objData.colliderType;
	switch (objData.colliderType)
	{
	case Collider_Plane:
	{
		pBody = std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation, objData.xmf3Rotation, objData.xmf3Scale, objData.nMass);
		std::shared_ptr<ColliderPlane> pColliderPlane;
		XMFLOAT3 direction = XMFLOAT3(objData.xmf4Orientation.x, objData.xmf4Orientation.y, objData.xmf4Orientation.z);
		pColliderPlane = std::make_shared<ColliderPlane>(pBody, direction, objData.xmf3Extents.x);
		g_ppColliderPlanes.emplace_back(pColliderPlane);
		pCollider = std::static_pointer_cast<Collider>(pColliderPlane);
	}
	break;

	case Collider_Box:
	{
		pBody = std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation, objData.xmf3Rotation, objData.xmf3Scale, objData.nMass);
		std::shared_ptr<ColliderBox> pColliderBox;
		pColliderBox = std::make_shared<ColliderBox>(pBody, objData.xmf3Extents);
		g_ppColliderBoxs.emplace_back(pColliderBox);
		pCollider = std::static_pointer_cast<Collider>(pColliderBox);
	}
	break;

	case Collider_Sphere:
	{
		pBody = std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation, objData.xmf3Rotation, objData.xmf3Scale, objData.nMass);
		std::shared_ptr<ColliderSphere> pColliderSphere;
		pColliderSphere = std::make_shared<ColliderSphere>(pBody, objData.xmf3Extents.x);
		g_ppColliderSpheres.emplace_back(pColliderSphere);
		pCollider = std::static_pointer_cast<Collider>(pColliderSphere);
	}
	break;

	case Collider_None:
	{
		pBody = std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation, objData.xmf3Rotation, objData.xmf3Scale, objData.nMass);
		//pBody->SetInGravity(false);
		pCollider = nullptr;

		m_xmf3Position = objData.xmf3Position;
		m_xmf4Orientation = objData.xmf4Orientation;
		m_xmf3Rotation = objData.xmf3Rotation;
		m_xmf3Scale = objData.xmf3Scale;
		m_Mass = objData.nMass;
	}

	default:
		break;
	}

	m_pBody = pBody;
	m_pCollider = pCollider;
	m_xmf3RenderOffsetPosition = objData.xmf3MeshOffsetPosition;
	m_xmf3RenderOffsetRotation = objData.xmf3MeshOffsetRotation;

#if defined(_DEBUG) && defined(COLLIDER_RENDER)
	if (m_pCollider) m_pCollider->BuildMesh(pd3dDevice, pd3dCommandList);
#endif
	BuildConstantBuffers(pd3dDevice);
	m_IntersectColliderRadius = 2.0f;
	m_IntersectCollider.Center = m_xmf3Position;
	m_IntersectCollider.Radius = m_IntersectColliderRadius;

	m_TraceColliderRadius = 20.0f;
	m_TraceCollider.Center = m_xmf3Position;
	m_TraceCollider.Radius = m_TraceColliderRadius;

	m_bShadow = objData.bShadow;
	UpdateTransform(nullptr);

	return true;
}

void Projectile::Animate(float elapsedTime)
{
}

void Projectile::Update(float elapsedTime)
{
	Object::Update(elapsedTime);

	if (!m_bDestroying)
	{
		m_IntersectCollider.Center = m_xmf3Position;
		m_TraceCollider.Center = m_xmf3Position;
		Intersect(elapsedTime);
		ChasingPlayer(elapsedTime);

		m_ElapsedLifeTime += elapsedTime;
		if (m_ElapsedLifeTime > m_ProjectileLifeTime)
			m_bDestroying = true;
	}

	// 0 미만인 경우
	if (m_xmf3Position.y < 0)
	{
		XMFLOAT3 xmf3Position = m_pBody->GetPosition();
		m_pBody->SetPosition(XMFLOAT3(xmf3Position.x, 0, xmf3Position.z));
		m_pBody->SetVelocity(XMFLOAT3(0, 0, 0));
		m_bDestroying = true;
	}
}

void Projectile::Intersect(float elapsedTime)
{
	float projectilePower = 5;
	XMFLOAT3 xmf3Direction = m_pBody->GetVelocity();
	XMVECTOR direction = XMLoadFloat3(&xmf3Direction);
	direction = XMVector3Normalize(direction);
	XMStoreFloat3(&xmf3Direction, direction);

#if defined(_DEBUG) || defined(DEBUG)

#endif

	switch (m_ProjectileProperty)
	{
	case Projectile_Player:
		// 몬스터 오브젝트와의 검사
		for (int i = 0; i < g_vpCharacters.size(); ++i)
		{
			if (g_vpCharacters[i]->GetInvincible() || g_vpCharacters[i]->GetObjectType() == Object_Player)
				continue;

			ColliderBox* objCollider = std::static_pointer_cast<ColliderBox>(g_vpCharacters[i]->GetCollider()).get();

			if (!objCollider)
				continue;

			if (!objCollider->GetIsActive() || m_pCollider.get() == objCollider)
				continue;

			if (m_IntersectCollider.Intersects(*objCollider->GetOBB()))
			{
				g_vpCharacters[i]->ApplyDamage(projectilePower, xmf3Direction);

				Player* pPlayer = (Player*)g_pPlayer.get();
				pPlayer->SetPlayerTargetObject(g_vpCharacters[i]);

				m_pBody->SetVelocity(XMFLOAT3(0, 0, 0));
				m_DissolveTime = 0;

				m_bDestroying = true;
				return;
			}
		}
		// 월드 오브젝트와의 검사 (움직이는, 고정된)
		for (int i = 0; i < g_vpWorldObjs.size(); ++i)
		{
			if (g_vpWorldObjs[i]->GetInvincible() || g_vpWorldObjs[i].get() == this || g_vpWorldObjs[i]->GetColliderType() != Collider_Box)
				continue;

			ColliderBox* objCollider = std::static_pointer_cast<ColliderBox>(g_vpWorldObjs[i]->GetCollider()).get();

			if (!objCollider)
				continue;

			if (!objCollider->GetIsActive() || m_pCollider.get() == objCollider)
				continue;

			if (m_IntersectCollider.Intersects(*objCollider->GetOBB()))
			{
				if (g_vpWorldObjs[i]->GetObjectType() == Object_Movable)
				{
					g_vpWorldObjs[i]->ApplyDamage(projectilePower, xmf3Direction);
					g_vpWorldObjs[i]->GetBody()->SetIsAwake(true);
				}

				m_pBody->SetVelocity(XMFLOAT3(0, 0, 0));
				m_DissolveTime = 0;

				m_bDestroying = true;
				return;
			}
		}
		break;
	case Projectile_Enemy:
	{
		// 플레이어와의 검사
		ColliderBox* playerCollider = (ColliderBox*)g_pPlayer->GetCollider().get();

		BoundingOrientedBox* tmp = playerCollider->GetOBB().get();

		if (m_IntersectCollider.Intersects(*tmp))
		{
			g_pPlayer->ApplyDamage(projectilePower, xmf3Direction);

			m_bDestroying = true;
			m_bVisible = false;
			return;
		}
		// 월드 오브젝트와의 검사 (움직이는, 고정된)
		for (int i = 0; i < g_vpWorldObjs.size(); ++i)
		{
			if (g_vpWorldObjs[i]->GetObjectType() == ObjectType::Object_Monster || g_vpWorldObjs[i]->GetColliderType() != Collider_Box)
				continue;

			if (g_vpWorldObjs[i]->GetInvincible() || g_vpWorldObjs[i].get() == this)
				continue;

			ColliderBox* objCollider = std::static_pointer_cast<ColliderBox>(g_vpWorldObjs[i]->GetCollider()).get();

			if (!objCollider)
				continue;

			if (!objCollider->GetIsActive() || m_pCollider.get() == objCollider)
				continue;

			if (m_IntersectCollider.Intersects(*objCollider->GetOBB()))
			{
				g_vpWorldObjs[i]->ApplyDamage(projectilePower, xmf3Direction);
				g_vpWorldObjs[i]->GetBody()->SetIsAwake(true);

				m_pBody->SetVelocity(XMFLOAT3(0, 0, 0));
				m_DissolveTime = 0;

				m_bDestroying = true;
				return;
			}
		}
	}
		break;

	default:
		break;
	}

}

void Projectile::ChasingPlayer(float elapsedTime)
{
	if (!m_bChasePlayer)
		return;

	// 플레이어와의 검사
	ColliderBox* playerCollider = (ColliderBox*)g_pPlayer->GetCollider().get();
	BoundingOrientedBox* tmp = playerCollider->GetOBB().get();

	// 너무 가까워진 경우 추적 해제
	if (m_TraceCollider.Intersects(*tmp))
	{
		m_bChasePlayer = false;
		return;
	}

	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR thisPosition = XMLoadFloat3(&m_xmf3Position);
	XMVECTOR velocity = XMVector3Normalize(playerPosition - thisPosition);
	velocity *= m_ProjectileSpeed;

	XMFLOAT3 xmf3Velocity;
	XMStoreFloat3(&xmf3Velocity, velocity);
	m_pBody->SetVelocity(xmf3Velocity);


}
