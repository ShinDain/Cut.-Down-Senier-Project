#include "../Header/Character.h"
#include "../../DirectXRendering/Header/Scene.h"

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
	if (!m_bIsAlive)
		return;

	// 오브젝트 파괴 타이머
	if (m_bDestroying)
	{
		if (m_bDissolveStart)
		{
			m_ElapsedDestroyTime += elapsedTime;
			m_DissolveValue = m_ElapsedDestroyTime / m_DestroyTime;
			if (m_DissolveValue > 0.9f)
			{
				float a = 100;
			}
			if (m_ElapsedDestroyTime >= m_DestroyTime)
			{
				//Cutting(XMFLOAT3(1, 0, 0));
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

	// 무적 시간 경과 누적
	if (m_bInvincible)
	{
		m_ElapsedInvincibleTime += elapsedTime;
		if (m_InvincibleTime <= m_ElapsedInvincibleTime)
		{
			m_ElapsedInvincibleTime = 0.0f;
			m_bInvincible = false;
		}
	}

	UpdateToRigidBody(elapsedTime);

	if (m_pObjectCB) UpdateObjectCB();

	if (m_pSibling) {
		m_pSibling->Update(elapsedTime);
	}
	if (m_pChild) {
		m_pChild->Update(elapsedTime);
	}

	ApplyCharacterFriction(elapsedTime);
	if(!m_bIsShoulderView)
		RotateToMove(elapsedTime);
	IsFalling();
	UpdateAnimationTrack(elapsedTime);

	if (m_bCrashWithObject)
	{
		ApplyDamage(m_CrashPower, m_xmf3CrashDirection);
		m_bCrashWithObject = false;
		m_CrashPower = 0;
		m_xmf3CrashDirection = { 0,0,0 };
	}
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
		//XMMATRIX xmmatOrientation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4RenderOrientation));
		XMMATRIX xmmatRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xmf3Rotation.x), XMConvertToRadians(m_xmf3Rotation.y), XMConvertToRadians(m_xmf3Rotation.z));
		XMMATRIX xmmatTranslate = XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
		// S * R * T
		//xmmatRotate = XMMatrixMultiply(xmmatRotate, xmmatOrientation);
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
	m_pAnimationController->SetTrackSpeed(nAnimationTrack, m_AnimationSpeed);
	m_pAnimationController->SetTrackRate(nAnimationTrack, 0);
	m_pAnimationController->SetTrackWeight(nAnimationTrack, 0);
	m_pAnimationController->SetTrackPosition(nAnimationTrack, 0);
}

void Character::IsFalling()
{
	if (m_xmf3RenderPosition.y < 0)
	{
		DoLanding();
		m_xmf3RenderPosition.y = 0;
	}
	else if (m_xmf3RenderPosition.y > 0)
	{
		ColliderBox* thisColliderBox = (ColliderBox*)m_pCollider.get();
		for (int i = 0; i < g_ppColliderBoxs.size(); ++i)
		{
			if (thisColliderBox == g_ppColliderBoxs[i].get())
				continue;

			BoundingOrientedBox obb = g_ppColliderBoxs[i]->GetOBB();
		
			XMVECTOR position = XMLoadFloat3(&m_xmf3RenderPosition);
			XMVECTOR direction = XMVectorSet(0, -1, 0, 0);
			float distance = 0;
			bool bIntersect = obb.Intersects(position, direction, distance);
			//if (distance < m_xmf3ColliderExtents.y * 10 && bIntersect)
			if (distance < 1 && bIntersect)
			{
				DoLanding();
				return;
			}
		}

		m_bIsFalling = true;
		if(m_bIsShoulderView)
			m_CharacterFriction = m_DefaultFriction;
		else
			m_CharacterFriction = 30.0f;
		m_Acceleration = 100.f;
		m_pBody->SetInGravity(true);
	}
}

void Character::DoLanding()
{
	m_bIsFalling = false; 
	m_MaxSpeedXZ = m_DefaultMaxSpeedXZ;
	m_CharacterFriction = m_DefaultFriction;
	m_Acceleration = m_DefaultAccel;
	m_pBody->SetInGravity(false);

	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	if(xmf3Velocity.y < 50 && xmf3Velocity.y > 0)
		xmf3Velocity.y = 0;
	m_pBody->SetVelocity(xmf3Velocity);
}

void Character::RotateToMove(float elapsedTime)
{
	// 진행 방향
	XMFLOAT3 xmf3TargetLook = m_pBody->GetAcceleration();
	XMVECTOR targetLook = XMLoadFloat3(&xmf3TargetLook);
	if (XMVectorGetX(XMVector3Length(targetLook)) <= 0)
		return;
	else
	{
		RotateToTargetLook(elapsedTime, xmf3TargetLook, 7);
	}

}

void Character::RotateToTargetLook(float elapsedTime, XMFLOAT3 xmf3TargetLook, float divideConst)
{
	XMVECTOR targetLook = XMLoadFloat3(&xmf3TargetLook);
	targetLook = XMVector3Normalize(targetLook);
	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR right = XMLoadFloat3(&m_xmf3Right);
	float angleBetweenLook = XMVectorGetX(XMVector3AngleBetweenVectors(targetLook, look));
	angleBetweenLook = XMConvertToDegrees(angleBetweenLook);

	angleBetweenLook *= m_TurnSpeed;

	if (!XMVectorGetX(XMVectorIsNaN(XMVectorReplicate(angleBetweenLook))))
	{
		if (XMVectorGetX(XMVector3Dot(targetLook, right)) < 0)
			angleBetweenLook *= -1;

		XMFLOAT3 xmf3Velcity = m_pBody->GetVelocity();
		xmf3Velcity.y = 0;
		XMVECTOR velocity = XMLoadFloat3(&xmf3Velcity);
		float velocityLength = XMVectorGetX(XMVector3Length(velocity));

		if (fabs(angleBetweenLook) > 10)
			angleBetweenLook = angleBetweenLook / divideConst;

		// 값이 너무 커지지 않도록
		float tmp = (int)(m_xmf3Rotation.y + angleBetweenLook) % 360;

		SetRotate(XMFLOAT3(m_xmf3Rotation.x, tmp, m_xmf3Rotation.z));
	}
}

void Character::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	if (m_bInvincible)
		return;
	// 피격 무적
	m_bInvincible = true;

	// 체력 감소
	m_HP -= power;

	XMVECTOR damageDirection = XMLoadFloat3(&xmf3DamageDirection);
	damageDirection = XMVector3Normalize(damageDirection);
	XMVECTOR deltaVelocity = damageDirection * power * 5;

	XMFLOAT3 xmf3DeltaVelocity;
	XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);

	m_pBody->AddVelocity(xmf3DeltaVelocity);
}

void Character::Cutting(XMFLOAT3 xmf3PlaneNormal)
{
	XMVECTOR orientation = XMQuaternionRotationRollPitchYaw(m_xmf3Rotation.x, m_xmf3Rotation.y, m_xmf3Rotation.z);
	XMStoreFloat4(&m_xmf4Orientation, orientation);

	m_bIsAlive = false;
	Scene::CreateCuttedObject(Scene::m_pd3dDevice, Scene::m_pd3dCommandList, this, 1, xmf3PlaneNormal, false);
	Scene::CreateCuttedObject(Scene::m_pd3dDevice, Scene::m_pd3dCommandList, this, -1, xmf3PlaneNormal, false);
}

void Character::BlendWithIdleMovement(float maxWeight)
{
	if (maxWeight <= 0)
	{
		UnableAnimationTrack(CHARACTER_IDLE_TRACK);
		UnableAnimationTrack(CHARACTER_MOVE_TRACK);
		return;
	}

	float weight = maxWeight;
	m_pAnimationController->SetTrackEnable(CHARACTER_IDLE_TRACK, true);
	m_pAnimationController->SetTrackEnable(CHARACTER_MOVE_TRACK, true);

	if (m_bIsShoulderView)
	{
		m_pAnimationController->SetTrackWeight(CHARACTER_IDLE_TRACK, weight);
		m_pAnimationController->SetTrackWeight(CHARACTER_MOVE_TRACK, 0);
		return;
	}

	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	xmf3Velocity.y = 0;
	XMVECTOR velocity = XMLoadFloat3(&xmf3Velocity);
	weight = XMVectorGetX(XMVector3Length(velocity)) / m_MaxSpeedXZ;

	if (weight < FLT_EPSILON)
		weight = 0;
	else if (weight > maxWeight)
		weight = maxWeight;
	m_pAnimationController->SetTrackWeight(CHARACTER_IDLE_TRACK, maxWeight - weight);
	m_pAnimationController->SetTrackWeight(CHARACTER_MOVE_TRACK, weight);
}

void Character::BlendIdleToAnimaiton(float curTrackRate, float goalRate, float mul, UINT nTrackIdx)
{
	// 시작 블랜드
	if (curTrackRate < goalRate)
	{
		float weight = curTrackRate * mul;

		m_pAnimationController->SetTrackWeight(nTrackIdx, weight);
		BlendWithIdleMovement(1 - weight);
	}
	else
	{
		m_pAnimationController->SetTrackWeight(nTrackIdx, 1);
		BlendWithIdleMovement(0);
	}
}

void Character::BlendAnimationToIdle(float curTrackRate, float startRate, float mul, UINT nTrackIdx)
{
	// 종료 블랜드
	if (curTrackRate > startRate)
	{
		float weight = (curTrackRate - startRate) * mul;
		
		m_pAnimationController->SetTrackWeight(nTrackIdx, 1 - weight);
		BlendWithIdleMovement(weight);
	}
}

void Character::BlendAnimationToAnimation(float curTrackRate, float startRate, float mul, UINT nTrackIdx1, UINT nTrackIdx2)
{
	// 시작 블랜드
	if (curTrackRate > startRate)
	{
		float weight = (curTrackRate - startRate) * mul;

		m_pAnimationController->SetTrackEnable(nTrackIdx1, true);
		m_pAnimationController->SetTrackEnable(nTrackIdx2, true);

		m_pAnimationController->SetTrackWeight(nTrackIdx1, 1 - weight);
		m_pAnimationController->SetTrackWeight(nTrackIdx2, weight);
	}
}

void Character::ApplyCharacterFriction(float elapsedTime)
{
	// 마찰력
	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	XMFLOAT3 xmf3VelocityXZ = XMFLOAT3(xmf3Velocity.x, 0, xmf3Velocity.z);
	XMVECTOR velocityXZ = XMLoadFloat3(&xmf3VelocityXZ);
	
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
	
	// 최대 속도 제한
	if (XMVectorGetX(XMVector3Length(velocityXZ)) > m_MaxSpeedXZ)
	{
		XMVECTOR direction = XMVector3Normalize(velocityXZ);
		velocityXZ = direction * m_MaxSpeedXZ;
		XMStoreFloat3(&newVelocity, velocityXZ);
		newVelocity.y = xmf3Velocity.y;
		m_pBody->SetVelocity(newVelocity);
	}
}

void Character::CrashWithObject(float crashPower, XMFLOAT3 xmf3CrashDirection)
{
	m_bCrashWithObject = true;
	//m_CrashPower = crashPower * 10;

	// 일단 20으로 고정
	m_CrashPower = 20;
	m_xmf3CrashDirection = xmf3CrashDirection;
}

///////////////////////////////////////////////////////////////
// 애니메이션 테스트용 클래스
///////////////////////////////////////////////////////////////

AnimTestCharacter::AnimTestCharacter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

AnimTestCharacter::~AnimTestCharacter()
{
	Destroy();
}

bool AnimTestCharacter::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	m_pAnimationController->SetTrackEnable(0, true);
	m_pAnimationController->SetTrackEnable(1, true);
	m_pAnimationController->SetTrackEnable(2, false);
	m_pAnimationController->SetTrackEnable(3, false);
	m_pAnimationController->SetTrackEnable(4, false);
	m_pAnimationController->SetTrackAnimationSet(0, m_tmpAnimationIdx);
	m_pAnimationController->SetTrackAnimationSet(1, m_tmpAnimationIdx);

	m_DestroyTime = 3.0f;
	m_DissolveTime = 0.0f;

	return true;
}

void AnimTestCharacter::Update(float elapsedTime)
{
	Character::Update(elapsedTime);

	BlendWithIdleMovement(1);
}

void AnimTestCharacter::KeyDownEvent(WPARAM wParam)
{
	if (wParam == 'L')
	{
		m_tmpAnimationIdx += 1;
		m_tmpAnimationIdx = m_tmpAnimationIdx % m_pAnimationController->m_pAnimationSets->m_nAnimationSets;

		m_pAnimationController->SetTrackAnimationSet(0, m_tmpAnimationIdx);
	}
}
