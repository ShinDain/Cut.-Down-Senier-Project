#include "../Header/Character.h"

Character::Character()
{
}

Character::Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks)
{
	std::shared_ptr<ModelDataInfo> pModelData = pModel;

	SetChild(pModelData->m_pRootObject);
	if (nAnimationTracks > 0)
		m_pAnimationController = std::make_unique<AnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pModelData);

	// RigidBody 생성
	std::shared_ptr<RigidBody> pBody =
		std::make_shared<RigidBody>(objData.xmf3Position, objData.xmf4Orientation, objData.xmf3Scale, objData.nMass);

	std::shared_ptr<Collider> pCollider;

	// 충돌체 타입에 따라 
	switch (objData.colliderType)
	{
	case Collider_Plane:
	{
		std::shared_ptr<ColliderPlane> pColliderPlane;
		pCollider = std::make_shared<ColliderPlane>(pBody, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0), objData.xmf3Extents.x);
		pCollider = std::static_pointer_cast<Collider>(pColliderPlane);
	}
	break;

	case Collider_Box:
	{
		std::shared_ptr<ColliderBox> pColliderBox;
		pColliderBox = std::make_shared<ColliderBox>(pBody, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), objData.xmf3Extents);
		pCollider = std::static_pointer_cast<Collider>(pColliderBox);
	}
	break;

	case Collider_Sphere:
	{
		std::shared_ptr<ColliderSphere> pColliderSphere;
		pColliderSphere = std::make_shared<ColliderSphere>(pBody, XMFLOAT3(0, 0, 0), objData.xmf3Extents.x);
		pCollider = std::static_pointer_cast<Collider>(pColliderSphere);
	}
	break;

	default:
		break;
	}

	pBody->SetPhysics(true);
	pBody->SetInGravity(true);
	pBody->SetIsCharacter(true);

	m_pBody = pBody;
	m_pCollider = pCollider;
	m_xmf3ColliderExtents = objData.xmf3Extents;

#if defined(_DEBUG)
	if (m_pCollider) m_pCollider->BuildMesh(pd3dDevice, pd3dCommandList);
#endif
	m_floorCheckRay.length = 3.f;
	m_floorCheckRay.xmf3Direction = XMFLOAT3(0, -1, 0);
	m_floorCheckRay.xmf3Start = XMFLOAT3(0, m_xmf3Position.y - m_xmf3ColliderExtents.y, 0);
}

Character::~Character()
{
}

void Character::Update(float elapsedTime)
{
	// RigidBody를 기준으로 위치를 갱신한다.
	if (m_pBody)
	{
		m_pBody->Update(elapsedTime);
		if (m_pBody->GetInvalid())
		{
			Destroy();
			return;
		}
		m_pCollider->UpdateWorldTransform();
		m_xmf3Position = m_pBody->GetPosition();
		m_xmf4Orientation = m_pBody->GetOrientation();
		m_xmf3Scale = m_pBody->GetScale();
		m_xmf4x4World = m_pBody->GetWorld();
	}

	UpdateTransform(NULL);

	ObjConstant objConstant;
	XMStoreFloat4x4(&objConstant.World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pObjectCB) m_pObjectCB->CopyData(0, objConstant);

	if (m_pSibling) {
		m_pSibling->Update(elapsedTime);
	}
	if (m_pChild) {
		m_pChild->Update(elapsedTime);
	}
}

void Character::Move(DWORD dwDirection, float distance)
{
	XMVECTOR direction = XMVectorZero();
	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR r = XMLoadFloat3(&m_xmf3Right);

	if (dwDirection & DIR_FORWARD)
	{
		direction = XMVectorAdd(direction, l);
	}
	if (dwDirection & DIR_BACKWARD)
	{
		direction = XMVectorAdd(direction, -l);
	}
	if (dwDirection & DIR_LEFT)
	{
		direction = XMVectorAdd(direction, -r);
	}
	if (dwDirection & DIR_RIGHT)
	{
		direction = XMVectorAdd(direction, r);
	}

	direction = XMVector3Normalize(direction);
	XMVECTOR deltaAccel = direction * distance;
	XMFLOAT3 xmf3deltaAccel;
	XMStoreFloat3(&xmf3deltaAccel, deltaAccel);

	m_pBody->SetAcceleration(xmf3deltaAccel);

	m_pBody->SetRotate(m_xmf3Rotate);
}

void Character::Rotate(float x, float y, float z)
{
	// x : Pitch, y : Yaw, z : Roll
	// 3인칭 카메라 기준
	if (x != 0.0f)
	{
		m_xmf3Rotate.x += x;
		if (m_xmf3Rotate.x > +89.0f) { x -= (m_xmf3Rotate.x - 89.0f); m_xmf3Rotate.x = +89.0f; }
		if (m_xmf3Rotate.x < -89.0f) { x -= (m_xmf3Rotate.x + 89.0f); m_xmf3Rotate.x = -89.0f; }
	}
	if (y != 0.0f)
	{
		m_xmf3Rotate.y += y;
		if (m_xmf3Rotate.y > 360.0f) m_xmf3Rotate.y -= 360.0f;
		if (m_xmf3Rotate.y < 0.0f) m_xmf3Rotate.y += 360.0f;
	}
	if (z != 0.0f)
	{
		m_xmf3Rotate.z += z;
		//if (m_xmf3Rotate.z > +20.0f) { z -= (m_xmf3Rotate.z - 20.0f); m_xmf3Rotate.z = +20.0f; }
		//if (m_xmf3Rotate.z < -20.0f) { z -= (m_xmf3Rotate.z + 20.0f); m_xmf3Rotate.z = -20.0f; }
	}
	if (y != 0.0f)
	{
		XMMATRIX xmmatRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
		XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
		XMVECTOR r = XMLoadFloat3(&m_xmf3Right);

		XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(l, xmmatRotate));
		XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(r, xmmatRotate));
	}
}

void Character::Jump()
{
	if (!IsFalling())
	{
		/*XMVECTOR deltaAccel = XMLoadFloat3(&m_pBody->GetAcceleration()) + (XMLoadFloat3(&m_xmf3Up) * 30000.f);
		XMFLOAT3 xmf3deltaAccel;
		XMStoreFloat3(&xmf3deltaAccel, deltaAccel);

		m_pBody->SetAcceleration(xmf3deltaAccel);*/

		XMVECTOR deltaVelocity = XMLoadFloat3(&m_pBody->GetVelocity()) + (XMLoadFloat3(&m_xmf3Up) * 50.f);
		XMFLOAT3 xmf3deltaVelocity;
		XMStoreFloat3(&xmf3deltaVelocity, deltaVelocity);

		m_pBody->SetVelocity(xmf3deltaVelocity);
		m_bCanJump = false;
	}
}

bool Character::IsFalling()
{
	XMVECTOR rayStart = XMLoadFloat3(&m_floorCheckRay.xmf3Start);
	rayStart = XMVector3TransformCoord(rayStart, XMLoadFloat4x4(&m_xmf4x4World));

	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	if (fabs(xmf3Velocity.y) > 3)
		return true;
	else
		return false;
}
