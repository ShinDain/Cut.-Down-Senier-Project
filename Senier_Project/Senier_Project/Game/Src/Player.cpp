#include "../Header/Player.h"
#include "../../DirectXRendering/Header/Scene.h"

#define PLAYER_IDLE_TRACK 0
#define PLAYER_MOVE_TRACK 1
#define PLAYER_SPRINT_TRACK 2
#define PLAYER_LOOP_TRACK 3
#define PLAYER_ONCE_TRACK_1 4
#define PLAYER_ONCE_TRACK_2 5
#define PLAYER_ONCE_TRACK_3 6

Player::Player()
{
}

Player::Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
			   ObjectInitData objData, 
			   std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

Player::~Player()
{
	Destroy();
}

bool Player::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
	m_pAnimationController->SetTrackEnable(PLAYER_IDLE_TRACK, true);	// idle
	m_pAnimationController->SetTrackEnable(PLAYER_MOVE_TRACK, true);	// move
	m_pAnimationController->SetTrackEnable(PLAYER_SPRINT_TRACK, false);	// sprint
	m_pAnimationController->SetTrackEnable(PLAYER_LOOP_TRACK, false);	// fall loop
	m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_1, false);	// once 1
	m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_2, false);	// once 2
	m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_3, false);	// once 3
	m_pAnimationController->m_vpAnimationTracks[PLAYER_ONCE_TRACK_1]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->m_vpAnimationTracks[PLAYER_ONCE_TRACK_2]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->m_vpAnimationTracks[PLAYER_ONCE_TRACK_3]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->SetTrackAnimationSet(PLAYER_IDLE_TRACK, Player_Anim_Index_Idle);
	m_pAnimationController->SetTrackAnimationSet(PLAYER_MOVE_TRACK, Player_Anim_Index_RunForward);
	m_pAnimationController->SetTrackAnimationSet(PLAYER_SPRINT_TRACK, Player_Anim_Index_Sprint);
	m_pAnimationController->SetTrackAnimationSet(PLAYER_LOOP_TRACK, Player_Anim_Index_Falling);

	m_ObjectSearchSphere.Center = m_xmf3Position;
	m_ObjectSearchSphere.Radius = 20.0f;

	m_JumpSpeed = 150.0f;

	return true;
}

void Player::Update(float elapsedTime)
{
	if (!m_bIsAlive)
		return;

	// 오브젝트 파괴 타이머
	if (m_bDestroying)
	{

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
	if (!m_bIsShoulderView)
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


	m_ObjectSearchSphere.Center = m_xmf3Position;
	if(m_pWeapon) m_pWeapon->Intersect(m_xmf3Look);

	if (m_bIsShoulderView)
	{
		m_pWeapon->SetVisible(false);
		m_MaxSpeedXZ = m_AimmingSpeed;
	}
	else
	{
		m_pWeapon->SetVisible(true);
		m_MaxSpeedXZ = m_DefaultSpeed;
	}

	//if (m_bDecreaseMaxSpeed)
	//{
	//	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	//	xmf3Velocity.y = 0;
	//	XMVECTOR velocity = XMLoadFloat3(&xmf3Velocity);
	//	float length = XMVectorGetX(XMVector3Length(velocity));
	//
	//	m_MaxSpeedXZ -= 1;
	//	if (m_MaxSpeedXZ < m_DefaultSpeed || length < m_DefaultSpeed)
	//	{
	//		m_bSprint = false;
	//		m_MaxSpeedXZ = m_DefaultSpeed;
	//		m_bDecreaseMaxSpeed = false;
	//	}
	//}
}

void Player::Destroy()
{
	Object::Destroy();

	if(m_pWeapon) m_pWeapon->Destroy();
	m_pWeapon.reset();
}

void Player::ProcessInput(UCHAR* pKeybuffer)
{
	if (m_bIgnoreInput)
		return;

	DWORD dwDirection = 0;
	if (pKeybuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeybuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeybuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeybuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;

	Move(dwDirection);
}


void Player::KeyDownEvent(WPARAM wParam)
{
	if (m_bIgnoreInput)
		return;

#if defined(_DEBUG) || defined(DEBUG)
	if (wParam == 'K')
	{
		ApplyDamage(100, XMFLOAT3(0, 0, -1));
	}
#endif

	if (!m_bIsAlive)
		return;

	switch (wParam)
	{
	case VK_SPACE:
		Jump();
		break;
	//case VK_SHIFT:
	//	m_bSprint = true;
	//	m_MaxSpeedXZ = m_SprintSpeed;
	//	break;
	default:
		break;
	}
}

void Player::KeyUpEvent(WPARAM wParam)
{
	switch (wParam)
	{
	//case VK_SHIFT:
	//	m_bDecreaseMaxSpeed = true;
	//	break;
	default:
		break;
	}
}

void Player::LeftButtonDownEvent()
{
	if (m_bIgnoreInput)
		return;

	// 좌클릭 공격
	Attack();
}

void Player::RightButtonDownEvent()
{
	if (m_bIgnoreInput)
		return;

}

void Player::Move(DWORD dwDirection)
{
	if (dwDirection == 0)
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		m_pBody->SetAcceleration(XMFLOAT3(0, xmf3Accel.y, 0));
	}
	else
	{
		XMVECTOR direction = XMVectorZero();

		XMMATRIX xmmatRotate = XMMatrixRotationY(XMConvertToRadians(m_xmf3CameraRotation.y));
		XMVECTOR camLook = XMVectorSet(0, 0, 1, 0);
		XMVECTOR camRight = XMVectorSet(1, 0, 0, 0);

		camLook = XMVector3TransformNormal(camLook, xmmatRotate);
		camRight = XMVector3TransformNormal(camRight, xmmatRotate);

		if (dwDirection & DIR_FORWARD)
		{
			direction = XMVectorAdd(direction, camLook);
		}
		if (dwDirection & DIR_BACKWARD)
		{
			direction = XMVectorAdd(direction, -camLook);
		}
		if (dwDirection & DIR_LEFT)
		{
			direction = XMVectorAdd(direction, -camRight);
		}
		if (dwDirection & DIR_RIGHT)
		{
			direction = XMVectorAdd(direction, camRight);
		}

		// 진행 방향
		direction = XMVector3Normalize(direction);

		XMVECTOR deltaAccel = direction * m_Acceleration;
		XMFLOAT3 xmf3deltaAccelXZ;
		XMStoreFloat3(&xmf3deltaAccelXZ, deltaAccel);

		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		xmf3Accel.x = xmf3deltaAccelXZ.x;
		xmf3Accel.z = xmf3deltaAccelXZ.z;
		m_pBody->SetAcceleration(xmf3Accel);
	}
}

void Player::Jump()
{
	if (m_bIsShoulderView)
		return;

	if (!m_bIsFalling || m_bCanDoubleJump)
	{
		if (m_bIsFalling)
			m_bCanDoubleJump = false;

		XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
		float length = xmf3Velocity.y;

		XMFLOAT3 xmf3Velcity = m_pBody->GetVelocity();
		xmf3Velocity.y = 0;
		XMVECTOR deltaVelocity = XMLoadFloat3(&xmf3Velocity) + (XMLoadFloat3(&m_xmf3Up) * m_JumpSpeed);
		XMFLOAT3 xmf3DeltaVel;
		XMStoreFloat3(&xmf3DeltaVel, deltaVelocity);
		m_pBody->SetVelocity(xmf3DeltaVel);

		ChangeToJumpState();
	}
}

void Player::ChangeToJumpState()
{
	m_nAnimationState = PlayerAnimationState::Player_State_Jump;
	UnableAnimationTrack(PLAYER_ONCE_TRACK_1);
	m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_1, true);
	m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, 1);

	m_pAnimationController->SetTrackEnable(PLAYER_IDLE_TRACK, false);
	m_pAnimationController->SetTrackEnable(PLAYER_MOVE_TRACK, false);
	m_pAnimationController->SetTrackEnable(PLAYER_SPRINT_TRACK, false);

	UnableAnimationTrack(PLAYER_LOOP_TRACK);
}

void Player::Attack()
{
	m_pAnimationController->SetTrackEnable(PLAYER_IDLE_TRACK, false);
	m_pAnimationController->SetTrackEnable(PLAYER_MOVE_TRACK, false);
	m_pAnimationController->SetTrackEnable(PLAYER_SPRINT_TRACK, false);

	// 숄더뷰 상태에선 던지기
	if (m_bIsShoulderView)
	{
		if (m_nAnimationState == Player_State_Idle)
		{
			UnableAnimationTrack(PLAYER_ONCE_TRACK_1);
			m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_1, true);
			m_nAnimationState = PlayerAnimationState::Player_State_Throw;
			m_pAnimationController->SetTrackAnimationSet(PLAYER_ONCE_TRACK_1, Player_Anim_Index_Throw);
			m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, 1);
			m_pAnimationController->SetTrackSpeed(PLAYER_ONCE_TRACK_1, 2.0f);
		}

		return;
	}

	switch (m_nAnimationState)
	{
	//case Player_State_Idle:
	//	break;
	case Player_State_Jump:
	case Player_State_Falling:
	case Player_State_Land:
	case Player_State_Throw:
		return;
	case Player_State_Melee:
	{
		// 선입력 방지
		float trackRate = m_pAnimationController->GetTrackRate(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack);
		if (trackRate < 0.25f || trackRate > 0.8f || m_nAttackCombo - m_nCurAttackTrack > 1)
			return;

		// 콤보 최대치 도달
		if (m_nAttackCombo == m_nMaxAttackCombo)
			return;
		m_nAttackCombo += 1;
		m_bCombeAttack = true;
	}
		break;
	default:
		m_nCurAttackTrack = 0;

		UnableAnimationTrack(PLAYER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_1, true);
		m_nAnimationState = PlayerAnimationState::Player_State_Melee;
		m_pAnimationController->SetTrackAnimationSet(PLAYER_ONCE_TRACK_1, Player_Anim_Index_MeleeOneHand);
		m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, 1);
		m_pAnimationController->SetTrackSpeed(PLAYER_ONCE_TRACK_1, 1.5f);
		break;
	}

	m_TurnSpeed = 1;
	m_Acceleration = 30.0f;
	// 공격 시 오브젝트 방향으로 자동 회전
	RotateToObj();
	m_TurnSpeed = 0;
	
	// 약간의 전진
	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR addVelocity = look * 20.f;
	XMFLOAT3 xmf3AddVelocity;
	XMStoreFloat3(&xmf3AddVelocity, addVelocity);
	m_pBody->AddVelocity(xmf3AddVelocity);
}

void Player::ThrowProjectile()
{
	if (!m_bCanThrow)
		return;

	m_bCanThrow = false;

	XMFLOAT3 xmf3ProjectilePos = m_xmf3Position;

	XMVECTOR cameraLook = XMVectorSet(0, 0, 1, 0);
	XMVECTOR cameraRight = XMVectorSet(1, 0, 0, 0);
	XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);
	XMVECTOR cameraRotation = XMLoadFloat3(&m_xmf3CameraRotation);
	XMMATRIX R = XMMatrixRotationY(XMConvertToRadians(m_xmf3CameraRotation.y));
	cameraLook = XMVector3TransformNormal(cameraLook, R);
	cameraRight = XMVector3TransformNormal(cameraRight, R);
	R = XMMatrixRotationAxis(cameraRight, XMConvertToRadians(m_xmf3CameraRotation.x));
	cameraLook = XMVector3TransformNormal(cameraLook, R);
	cameraUp = XMVector3TransformNormal(cameraUp, R);

	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR right = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR projectilePos = XMLoadFloat3(&xmf3ProjectilePos);

	projectilePos += look * 3;
	projectilePos += right * 4;
	projectilePos += cameraUp * m_xmf3ColliderExtents.y * 13;
	XMStoreFloat3(&xmf3ProjectilePos, projectilePos);
	XMVECTOR projectileTarget = projectilePos;
	projectileTarget += cameraLook * 10;
	projectileTarget += cameraUp;
	XMVECTOR projectileVelocity = projectileTarget - projectilePos;
	projectileVelocity = XMVector3Normalize(projectileVelocity);

	XMFLOAT3 xmf3ProjectileVelocity;
	projectileVelocity = projectileVelocity * 300;

	XMVECTOR projectileOrientation = XMLoadFloat4(&m_xmf4Orientation);
	projectileOrientation = XMQuaternionRotationRollPitchYaw(0, 0, 90);
	XMFLOAT4 xmf4projectileOrientation;
	XMStoreFloat4(&xmf4projectileOrientation, projectileOrientation);
	std::shared_ptr<Object> tmp = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList, xmf3ProjectilePos, xmf4projectileOrientation,
		XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), PLAYER_PROJECTILE_MODEL_NAME, 0);
	XMStoreFloat3(&xmf3ProjectileVelocity, projectileVelocity);
	tmp->GetBody()->SetVelocity(xmf3ProjectileVelocity);
}

void Player::RotateToObj()
{
	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	//xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);
	
	float closestDistance = 9999;
	int closestIdx = -1;
	for (int i = 0; i < g_vpMovableObjs.size(); ++i)
	{
		XMFLOAT3 xmf3TmpPosition = g_vpMovableObjs[i]->GetPosition();
		//xmf3TmpPosition.y = 0;
		XMVECTOR tmpPosition = XMLoadFloat3(&xmf3TmpPosition);

		float distance = XMVectorGetX(XMVector3Length(tmpPosition - myPosition));
		if (distance < closestDistance)
		{
			closestDistance = distance;
			closestIdx = i;
		}
	}
	if (closestDistance > m_AttackRange)
		return;

	xmf3MyPosition.y = 0;
	myPosition = XMLoadFloat3(&xmf3MyPosition);

	m_pTargetObject = g_vpMovableObjs[closestIdx];
	XMFLOAT3 xmf3TargetPosition = g_vpMovableObjs[closestIdx]->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);
	XMFLOAT3 xmf3TargetLook;
	XMStoreFloat3(&xmf3TargetLook, targetPosition - myPosition);

	RotateToTargetLook(0.0f, xmf3TargetLook, 1);

	// 약간의 전진 -> 몬스터와의 거리에 따라 조절 필요
	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR addVelocity = look * closestDistance * 2.0f;
	XMFLOAT3 xmf3AddVelocity;
	XMStoreFloat3(&xmf3AddVelocity, addVelocity);
	m_pBody->AddVelocity(xmf3AddVelocity);
}

void Player::AcquireItem(UINT itemType)
{
	switch (itemType)
	{
	default:
		m_nScore += 100;
		break;
	}
}

// 동작이 캔슬되어 초기화될 시
void Player::InitializeState()
{
	UnableAnimationTrack(PLAYER_IDLE_TRACK);
	UnableAnimationTrack(PLAYER_MOVE_TRACK);
	UnableAnimationTrack(PLAYER_LOOP_TRACK);
	UnableAnimationTrack(PLAYER_ONCE_TRACK_1);
	UnableAnimationTrack(PLAYER_ONCE_TRACK_2);
	UnableAnimationTrack(PLAYER_ONCE_TRACK_3);

	m_pWeapon->SetActive(false);
	m_bIgnoreInput = false;

	m_nAttackCombo = 0;
	m_bCombeAttack = false;

	m_bIsFalling = false;
	m_bCanDoubleJump = true;
	m_bCanThrow = true;
	m_MaxSpeedXZ = m_DefaultSpeed;
	m_CharacterFriction = 350.0f;

	m_Acceleration = 500.0f;
	m_TurnSpeed = 1;

	// 일반 상태로 강제 초기화
	m_nAnimationState = Player_State_Idle;
	BlendWithIdleMovement(1);
}

void Player::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	if (m_bInvincible || m_nAnimationState == Player_State_Death)
		return;

	Object::ApplyDamage(power, xmf3DamageDirection);

	m_bIgnoreInput = true;

	XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
	xmf3Accel.x = 0;
	xmf3Accel.z = 0;
	m_pBody->SetAcceleration(xmf3Accel);

	m_pAnimationController->SetTrackEnable(PLAYER_IDLE_TRACK, false);
	m_pAnimationController->SetTrackEnable(PLAYER_MOVE_TRACK, false);
	m_pAnimationController->SetTrackEnable(PLAYER_SPRINT_TRACK, false);

	if (m_HP > 0)
	{
		m_nAnimationState = PlayerAnimationState::Player_State_Hit;
		UnableAnimationTrack(PLAYER_LOOP_TRACK);
		UnableAnimationTrack(PLAYER_ONCE_TRACK_1);
		UnableAnimationTrack(PLAYER_ONCE_TRACK_2);
		UnableAnimationTrack(PLAYER_ONCE_TRACK_3);
		m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(PLAYER_ONCE_TRACK_1, Player_Anim_Index_GetHit);
		m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, 1);
	}
	else
	{
		m_nAnimationState = PlayerAnimationState::Player_State_Death;
		UnableAnimationTrack(PLAYER_LOOP_TRACK);
		UnableAnimationTrack(PLAYER_ONCE_TRACK_1);
		UnableAnimationTrack(PLAYER_ONCE_TRACK_2);
		UnableAnimationTrack(PLAYER_ONCE_TRACK_3);
		m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(PLAYER_ONCE_TRACK_1, Player_Anim_Index_Death);
		m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, 1);
	}
	
}

void Player::DoLanding()
{
	switch (m_nAnimationState)
	{
	//case Player_State_Idle:
	//	break;
	case Player_State_Jump:
		m_pAnimationController->SetTrackEnable(PLAYER_LOOP_TRACK, true);
		m_pAnimationController->SetTrackAnimationSet(PLAYER_LOOP_TRACK, Player_Anim_Index_Falling);

		UnableAnimationTrack(PLAYER_ONCE_TRACK_1);

		m_pAnimationController->SetTrackWeight(PLAYER_LOOP_TRACK, 1);
		m_nAnimationState = PlayerAnimationState::Player_State_Falling;
		return;
	case Player_State_Falling:
		m_pAnimationController->SetTrackAnimationSet(PLAYER_ONCE_TRACK_1, Player_Anim_Index_JumpDown);
		m_nAnimationState = PlayerAnimationState::Player_State_Land;
		break;
	//case Player_State_Land:
	//	break;
	case Player_State_Melee:
		Character::DoLanding();
		m_Acceleration = 30.0f;
		m_TurnSpeed = 0;
		m_bCanDoubleJump = true;
		return;
	default:
		break;
	}

	Character::DoLanding();

	//m_bIsFalling = false;
	//m_MaxSpeedXZ = 100.f;
	//m_CharacterFriction = 350.0f;
	//m_Acceleration = 500.0f;
	//m_pBody->SetInGravity(false);
	m_bCanDoubleJump = true;
	m_TurnSpeed = 1;


}

void Player::UpdateAnimationTrack(float elapsedTime)
{
	if(m_pWeapon) m_pWeapon->SetActive(false);

	switch (m_nAnimationState)
	{
	case Player_State_Idle:
	{
		// 바닥에서의 기본 움직임
		BlendWithIdleMovement(1);
	}
		break;
	case Player_State_Jump:
	{
		m_pAnimationController->SetTrackAnimationSet(PLAYER_ONCE_TRACK_1, Player_Anim_Index_JumpUp);
		// 낙하 애니메이션과 블랜딩
		if (m_pAnimationController->GetTrackRate(PLAYER_ONCE_TRACK_1) > 0.8f)
		{
			m_pAnimationController->SetTrackEnable(PLAYER_LOOP_TRACK, true);
			m_pAnimationController->SetTrackAnimationSet(PLAYER_LOOP_TRACK, Player_Anim_Index_Falling);

			float weight = (m_pAnimationController->GetTrackRate(PLAYER_ONCE_TRACK_1) - 0.8f) * 5;
			m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, 1 - weight);
			m_pAnimationController->SetTrackWeight(PLAYER_LOOP_TRACK, weight);
		}
		// jump_up 애니메이션 종료
		if (m_pAnimationController->GetTrackOver(PLAYER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(PLAYER_ONCE_TRACK_1);

			m_pAnimationController->SetTrackWeight(PLAYER_LOOP_TRACK, 1);
			m_nAnimationState = PlayerAnimationState::Player_State_Falling;
		}
	}
		break;

	case Player_State_Falling:
		break;
	case Player_State_Land:
	{
		m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_1, true);

		float trackRate = m_pAnimationController->GetTrackRate(PLAYER_ONCE_TRACK_1);
		if (trackRate > 0.5f)
		{
			m_pAnimationController->SetTrackEnable(PLAYER_LOOP_TRACK, false);

			float weight = (trackRate - 0.5f) * 2.f;
			m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, 1 - weight);

			BlendWithIdleMovement(weight);
		}
		else
		{
			float weight = trackRate * 2;
			m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, weight);
			m_pAnimationController->SetTrackWeight(PLAYER_LOOP_TRACK, 1 - weight);
		}

		if (m_pAnimationController->GetTrackOver(PLAYER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(PLAYER_ONCE_TRACK_1);

			m_nAnimationState = PlayerAnimationState::Player_State_Idle;
		}
	}
	break;

	case Player_State_Melee:
	{
		float trackRate = m_pAnimationController->GetTrackRate(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack);
		if (trackRate > 0.6f)
		{	
			// 추가 콤보 입력 시 다음 동작으로 블랜딩
			if (m_bCombeAttack)
			{
				float weight = (trackRate - 0.6) * 2.5f;

				m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack + 1, true);
				m_pAnimationController->SetTrackAnimationSet(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack + 1, Player_Anim_Index_MeleeTwoHand);
				m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack + 1, weight);
				m_pAnimationController->SetTrackSpeed(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack + 1, 1.8f);

				m_pAnimationController->SetTrackEnable(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack, true);
				m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack, 1 - weight);
				m_pAnimationController->SetTrackSpeed(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack, 1.8f);
			}
			// 추가 입력 없는 경우 마무리 동작으로 블랜딩
			else
			{
				float weight = (trackRate - 0.6f) * 2.5f;
				m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack, 1 - weight);

				BlendWithIdleMovement(weight);
			}
		}

		else if(trackRate > 0.2f)
		{
			m_pWeapon->SetActive(true);
		}

		// 현재 공격 동작 마무리 시
		if (m_pAnimationController->GetTrackOver(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack))
		{
			// 추가 입력 시 
			if (m_bCombeAttack)
			{
				if(m_nCurAttackTrack == m_nAttackCombo - 1)
					m_bCombeAttack = false;
				UnableAnimationTrack(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack);
				m_nCurAttackTrack = m_nCurAttackTrack + 1;
				m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1 + m_nCurAttackTrack, 1);
				break;
			}

			// 모든 동작 마무리 후 기본 상태로 전환
			UnableAnimationTrack(PLAYER_ONCE_TRACK_1);
			UnableAnimationTrack(PLAYER_ONCE_TRACK_2);
			UnableAnimationTrack(PLAYER_ONCE_TRACK_3);

			m_nAnimationState = PlayerAnimationState::Player_State_Idle;
			m_nAttackCombo = 0;
			// 가속도 정상화
			m_Acceleration = 500.0f;
			m_TurnSpeed = 1;
		}
	}
		break;

	case Player_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(PLAYER_ONCE_TRACK_1);
		if (trackRate > 0.6f)
		{
			float weight = (trackRate - 0.6f) * 2.5f;
			m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, 1 - weight);

			BlendWithIdleMovement(weight);
			m_bIgnoreInput = false;
		}
		if (m_pAnimationController->GetTrackOver(PLAYER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(PLAYER_ONCE_TRACK_1);

			m_nAnimationState = PlayerAnimationState::Player_State_Idle;
		}
	}
		break;

	case Player_State_Death:
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		m_pBody->SetAcceleration(XMFLOAT3(0, xmf3Accel.y, 0));

		if (m_pAnimationController->GetTrackOver(PLAYER_ONCE_TRACK_1))
		{
			m_ElapsedDestroyTime += elapsedTime;
			//if (m_ElapsedDestroyTime > m_DestroyTime)
			//	m_bIsAlive = false;
		}
	}
		break;

	case Player_State_Throw:
	{
		float trackRate = m_pAnimationController->GetTrackRate(PLAYER_ONCE_TRACK_1);

		if (trackRate > 0.4f)
		{
			ThrowProjectile();

			float weight = (trackRate - 0.4f) * (5.0f / 3.0f);
			m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, 1 - weight);

			BlendWithIdleMovement(weight);
			
		}
		else if (trackRate < 0.2f)
		{
			float weight = trackRate * 5.0f;

			m_pAnimationController->SetTrackWeight(PLAYER_ONCE_TRACK_1, weight);
			BlendWithIdleMovement(1 - weight);
		}

		// 현재 공격 동작 마무리 시
		if (m_pAnimationController->GetTrackOver(PLAYER_ONCE_TRACK_1))
		{
			m_bCanThrow = true;

			// 모든 동작 마무리 후 기본 상태로 전환
			UnableAnimationTrack(PLAYER_ONCE_TRACK_1);
			UnableAnimationTrack(PLAYER_ONCE_TRACK_2);
			UnableAnimationTrack(PLAYER_ONCE_TRACK_3);

			m_nAnimationState = PlayerAnimationState::Player_State_Idle;
		}
	}
	break;

	default:
		break;
	}
}

void Player::BlendWithIdleMovement(float maxWeight)
{
	if(m_bIsShoulderView)
		m_pAnimationController->SetTrackAnimationSet(PLAYER_IDLE_TRACK, Player_Anim_Index_ThrowIdle);
	else
		m_pAnimationController->SetTrackAnimationSet(PLAYER_IDLE_TRACK, Player_Anim_Index_Idle);

	Character::BlendWithIdleMovement(maxWeight);

	//m_pAnimationController->SetTrackEnable(PLAYER_IDLE_TRACK, true);
	//m_pAnimationController->SetTrackEnable(PLAYER_MOVE_TRACK, true);
	//if (m_bSprint)
	//	m_pAnimationController->SetTrackEnable(PLAYER_SPRINT_TRACK, true);
	//else
	//	m_pAnimationController->SetTrackEnable(PLAYER_SPRINT_TRACK, false);
	//
	//float weight = maxWeight;
	//XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	//xmf3Velocity.y = 0;
	//XMVECTOR velocity = XMLoadFloat3(&xmf3Velocity);
	//float length = XMVectorGetX(XMVector3Length(velocity));
	//
	//if (m_bSprint)
	//	weight = length / m_SprintSpeed;
	//else
	//	weight = length / m_DefaultSpeed;
	//
	//if (weight < FLT_EPSILON)
	//	weight = 0;
	//else if (weight > maxWeight)
	//	weight = maxWeight;
	//if (m_bSprint)
	//{
	//	if (weight <= 0.66f)
	//	{
	//		m_pAnimationController->SetTrackWeight(PLAYER_IDLE_TRACK, maxWeight - (weight / 2 * 3));
	//		m_pAnimationController->SetTrackWeight(PLAYER_MOVE_TRACK, weight / 2 * 3);
	//		m_pAnimationController->SetTrackWeight(PLAYER_SPRINT_TRACK, 0);
	//	}
	//	else
	//	{
	//		m_pAnimationController->SetTrackWeight(PLAYER_IDLE_TRACK, 0);
	//		m_pAnimationController->SetTrackWeight(PLAYER_MOVE_TRACK, maxWeight - (weight - 0.66f) * 3);
	//		m_pAnimationController->SetTrackWeight(PLAYER_SPRINT_TRACK, (weight - 0.66f) * 3);
	//	}
	//}
	//else
	//{
	//	m_pAnimationController->SetTrackWeight(PLAYER_IDLE_TRACK, maxWeight - weight);
	//	m_pAnimationController->SetTrackWeight(PLAYER_MOVE_TRACK, weight);
	//}
}
