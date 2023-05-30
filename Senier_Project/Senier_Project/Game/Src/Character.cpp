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

	m_pBody->SetIsCharacter(true);

	m_Acceleration = 500.0f;

	return true;
}

void Character::Update(float elapsedTime)
{
	Object::Update(elapsedTime);

	ApplyCharacterFriction(elapsedTime);
	RotateToMove(elapsedTime);
	IsFalling();
	UpdateAnimationTrack();

	// 속도 및 위치 변화
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

void Character::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	// Animate 후에 호출되어 Bone 행렬을 갱신
	// 
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
		//XMMATRIX xmmatOrientation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4Orientation));
		XMMATRIX xmmatRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotation.x), XMConvertToRadians(m_xmf3Rotation.y), XMConvertToRadians(m_xmf3Rotation.z));
		XMMATRIX xmmatTranslate = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
		// S * R * T
		//world = XMMatrixMultiply(xmmatScale, XMMatrixMultiply(xmmatOrientation, xmmatTranslate));
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

	if (m_pCollider) m_pCollider->UpdateWorldTransform();

	if (m_pSibling) {
		m_pSibling->UpdateTransform(pxmf4x4Parent);
	}
	if (m_pChild) {
		m_pChild->UpdateTransform(&m_xmf4x4World);
	}

}

void Character::UnableAnimationTrack(int nAnimationTrack)
{
	// Track default화

	m_pAnimationController->SetTrackOver(nAnimationTrack, false);
	m_pAnimationController->SetTrackEnable(nAnimationTrack, false);
	m_pAnimationController->SetTrackRate(nAnimationTrack, 0);
	m_pAnimationController->SetTrackWeight(nAnimationTrack, 0);
	m_pAnimationController->SetTrackPosition(nAnimationTrack, 0);
}

void Character::IsFalling()
{
	if (m_xmf3RenderPosition.y < 0)
	{
		m_xmf3RenderPosition.y = 0;
		DoLanding();
	}
	else if (m_xmf3RenderPosition.y > 0)
	{
		for (int i = 1; i < g_ppColliderBoxs.size(); ++i)
		{
			BoundingOrientedBox obb = g_ppColliderBoxs[i]->GetOBB();
			
			XMVECTOR position = XMLoadFloat3(&m_xmf3RenderPosition);
			XMVECTOR direction = XMVectorSet(0, -1, 0, 0);
			float distance = 100;
			bool bIntersect = obb.Intersects(position, direction, distance);
			if (distance < m_xmf3ColliderExtents.y * 10 && bIntersect)
			{
				DoLanding();
				return;
			}
		}

		m_bIsFalling = true;
		m_CharacterFriction = 30.f;
		m_Acceleration = 100.f;
	}
}

void Character::DoLanding()
{
	m_bIsFalling = false;
	m_MaxSpeedXZ = 100.f;
	m_CharacterFriction = 350.0f;;
	m_Acceleration = 500.0f;
}

void Character::RotateToMove(float elapsedTime)
{
	// 진행 방향
	XMVECTOR l = XMVectorSet(0, 0, 1, 0);
	XMVECTOR targetLook = XMLoadFloat3(&m_pBody->GetAcceleration());
	if (XMVectorGetX(XMVector3Length(targetLook)) <= 0)
		return;

	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR right = XMLoadFloat3(&m_xmf3Right);
	float angleBetweenLook = XMVectorGetX(XMVector3AngleBetweenVectors(targetLook, look));
	angleBetweenLook = XMConvertToDegrees(angleBetweenLook);

	angleBetweenLook *= m_Acceleration / 500;

	if (!XMVectorGetX(XMVectorIsNaN(XMVectorReplicate(angleBetweenLook))))
	{
		if (XMVectorGetX(XMVector3Dot(targetLook, right)) < 0)
			angleBetweenLook *= -1;

		XMFLOAT3 xmf3Velcity = m_pBody->GetVelocity();
		xmf3Velcity.y = 0;
		XMVECTOR velocity = XMLoadFloat3(&xmf3Velcity);
		float velocityLength = XMVectorGetX(XMVector3Length(velocity));

		if(angleBetweenLook > 5)
			angleBetweenLook = angleBetweenLook / 5;

		// 값이 너무 커지지 않도록
		float tmp = (int)(m_xmf3Rotation.y + angleBetweenLook) % 360;

		SetRotate(XMFLOAT3(m_xmf3Rotation.x, tmp, m_xmf3Rotation.z));
	}
}

void Character::BlendWithIdleMovement(float maxWeight)
{
	m_pAnimationController->SetTrackEnable(0, true);
	m_pAnimationController->SetTrackEnable(1, true);

	float weight = maxWeight;
	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	xmf3Velocity.y = 0;
	XMVECTOR velocity = XMLoadFloat3(&xmf3Velocity);
	weight = XMVectorGetX(XMVector3Length(velocity)) / m_MaxSpeedXZ;

	if (weight < 0.1f)
		weight = 0;
	else if (weight > maxWeight)
		weight = maxWeight;
	m_pAnimationController->SetTrackWeight(0, maxWeight - weight);
	m_pAnimationController->SetTrackWeight(1, weight);
}

void Character::ApplyCharacterFriction(float elapsedTime)
{
	// 마찰력
	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	XMFLOAT3 xmf3VelocityXZ = XMFLOAT3(xmf3Velocity.x, 0, xmf3Velocity.z);
	XMVECTOR velocityXZ = XMLoadFloat3(&xmf3VelocityXZ);
	// 최대 속도 제한
	if (XMVectorGetX(XMVector3Length(velocityXZ)) > m_MaxSpeedXZ)
	{
		XMVECTOR direction = XMVector3Normalize(velocityXZ);
		velocityXZ = direction * m_MaxSpeedXZ;
		XMFLOAT3 newVelocity;
		XMStoreFloat3(&newVelocity, velocityXZ);
		newVelocity.y = xmf3Velocity.y;
		m_pBody->SetVelocity(newVelocity);
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
		m_pBody->SetVelocity(newVelocity);
	}
}

void Character::CalcVelocityAndPosition(float elapsedTime)
{
	//XMVECTOR velocity = XMLoadFloat3(&m_xmf3Velocity);

	//// 중력 적용
	//ApplyGravity(elapsedTime);
	//
	//// 가속도에 따른 속도 변화
	//XMVECTOR deltaVel = XMLoadFloat3(&m_xmf3Acceleration) * elapsedTime;
	//velocity += deltaVel;
	//XMStoreFloat3(&m_xmf3Velocity, velocity);

	//// 위치 변화 계산 후 적용
	//XMVECTOR resultPosition = velocity * elapsedTime + XMLoadFloat3(&m_xmf3Position);
	//XMStoreFloat3(&m_xmf3Position, resultPosition);
	//	

	//// 최대 속도 제한 및 마찰력 
	//ApplyCharacterFriction(elapsedTime);
}
