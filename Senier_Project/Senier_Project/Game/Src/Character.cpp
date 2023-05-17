#include "../Header/Character.h"

Character::Character()
{
}

Character::Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
					 ObjectInitData objData,
					 std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

Character::~Character()
{
	Object::Destroy();
}

bool Character::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Object::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	m_pCollider->SetOffsetPosition(XMFLOAT3(0, objData.xmf3Extents.y, 0));

	m_Acceleration = 500.0f;
	m_floorCheckRay.length = 3.f;
	m_floorCheckRay.xmf3Direction = XMFLOAT3(0, -1, 0);
	m_floorCheckRay.xmf3Start = XMFLOAT3(0, m_xmf3Position.y - m_xmf3ColliderExtents.y, 0);

	return true;
}

void Character::Update(float elapsedTime)
{
	Object::Update(elapsedTime);

	// �ӵ� �� ��ġ ��ȭ
	//IsFalling();
	//CalcVelocityAndPosition(elapsedTime);
	/*UpdateToRigidBody(elapsedTime);
	
	ObjConstant objConstant;
	XMStoreFloat4x4(&objConstant.World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pObjectCB) m_pObjectCB->CopyData(0, objConstant);

	if (m_pSibling) {
		m_pSibling->Update(elapsedTime);
	}
	if (m_pChild) {
		m_pChild->Update(elapsedTime);
	}*/
}

void Character::IsFalling()
{
	// �÷��� ���� �ö� �ִٸ� 
	// false��, 
	
	if (m_xmf3Position.y > 0)
	{
		m_bIsFalling = true;
		m_MaxSpeedXZ = 75.f;
		//m_Acceleration = 50.0f;
		//m_CharacterFriction = 75.0f;
	}
	else if(m_xmf3Position.y < 0)
	{
		//m_Acceleration = 500.0f;
		//m_CharacterFriction = 350.0f;
		m_MaxSpeedXZ = 100.f;
		m_xmf3Position.y = 0;
		m_xmf3Velocity.y = 0;
		m_bIsFalling = false;
	}
}

void Character::ApplyCharacterFriction(float elapsedTime)
{
	// ������
	XMFLOAT3 xmf3Velocity = m_xmf3Velocity;
	XMFLOAT3 xmf3VelocityXZ = XMFLOAT3(xmf3Velocity.x, 0, xmf3Velocity.z);
	XMVECTOR velocityXZ = XMLoadFloat3(&xmf3VelocityXZ);
	// �ִ� �ӵ� ����
	if (XMVectorGetX(XMVector3Length(velocityXZ)) > m_MaxSpeedXZ)
	{
		XMVECTOR direction = XMVector3Normalize(velocityXZ);
		velocityXZ = direction * m_MaxSpeedXZ;
		XMFLOAT3 newVelocity;
		XMStoreFloat3(&newVelocity, velocityXZ);
		newVelocity.y = xmf3Velocity.y;
		m_xmf3Velocity = newVelocity;
	}
	else
	{
		XMVECTOR direction = XMVector3Normalize(velocityXZ);
		XMVECTOR friction = -direction;
		friction = m_CharacterFriction * friction * elapsedTime;

		if (XMVectorGetX(XMVector3Length(friction)) > XMVectorGetX(XMVector3Length(velocityXZ)))
			friction = XMVector3Normalize(friction) * XMVectorGetX(XMVector3Length(velocityXZ));

		velocityXZ = velocityXZ + friction;
		XMFLOAT3 newVelocity;
		XMStoreFloat3(&newVelocity, velocityXZ);
		newVelocity.y = xmf3Velocity.y;
		m_xmf3Velocity = newVelocity;
	}
}

void Character::CalcVelocityAndPosition(float elapsedTime)
{
	XMVECTOR velocity = XMLoadFloat3(&m_xmf3Velocity);

	// �߷� ����
	ApplyGravity(elapsedTime);
	
	// ���ӵ��� ���� �ӵ� ��ȭ
	XMVECTOR deltaVel = XMLoadFloat3(&m_xmf3Acceleration) * elapsedTime;
	velocity += deltaVel;
	XMStoreFloat3(&m_xmf3Velocity, velocity);

	// ��ġ ��ȭ ��� �� ����
	XMVECTOR resultPosition = velocity * elapsedTime + XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, resultPosition);
		

	// �ִ� �ӵ� ���� �� ������ 
	ApplyCharacterFriction(elapsedTime);
}
