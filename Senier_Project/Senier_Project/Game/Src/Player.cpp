#include "../Header/Player.h"

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
	m_pAnimationController->SetTrackEnable(0, true);
	m_pAnimationController->SetTrackEnable(1, true);
	m_pAnimationController->SetTrackEnable(2, false);
	m_pAnimationController->SetTrackEnable(3, false);
	m_pAnimationController->m_vpAnimationTracks[2]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->SetTrackAnimationSet(0, Player_Anim_Index_Idle);
	m_pAnimationController->SetTrackAnimationSet(1, Player_Anim_Index_RunForward);
	m_pAnimationController->SetTrackAnimationSet(3, Player_Anim_Index_Falling);


	return true;
}

void Player::Update(float elapsedTime)
{
	Character::Update(elapsedTime);

	RotateToMove(elapsedTime);

	m_pWeapon->Intersect(m_xmf3Look);
}

void Player::Destroy()
{
	Object::Destroy();

	//if(m_pWeapon) m_pWeapon->Destroy();
	//m_pWeapon.reset();
}

void Player::ProcessInput(UCHAR* pKeybuffer)
{
	if (m_nAnimationState == PlayerAnimationState::Player_State_Death)
		return;

	DWORD dwDirection = 0;
	if (pKeybuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeybuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeybuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeybuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;


	Move(dwDirection);

	if (pKeybuffer[VK_LBUTTON] & 0xF0)	Attack();
}


void Player::KeyDownEvent(WPARAM wParam)
{
	if (wParam == 'K')
	{
		ApplyDamage(10, XMFLOAT3(0, 0, -1));
	}

	if (!m_bIsAlive)
		return;

	if(wParam == VK_SPACE)
		Jump();
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

		if (m_Acceleration < 30.0f)
		{
			float a = 0;
		}
		if (m_CharacterFriction < 100.0f)
		{
			float a = 0;
		}

		// 진행 방향
		direction = XMVector3Normalize(direction);
		XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
		XMVECTOR right = XMLoadFloat3(&m_xmf3Right);

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
	UnableAnimationTrack(2);
	m_pAnimationController->SetTrackEnable(2, true);
	m_pAnimationController->SetTrackWeight(2, 1);

	m_pAnimationController->SetTrackEnable(0, false);
	m_pAnimationController->SetTrackEnable(1, false);

	UnableAnimationTrack(3);
}

void Player::Attack()
{
	switch (m_nAnimationState)
	{
	//case Player_State_Idle:
	//	break;
	//case Player_State_Jump:
	//	break;
	//case Player_State_Falling:
	//	break;
	//case Player_State_Land:
	//	break;
	case Player_State_Melee:
		return;
		break;
	default:
		m_pAnimationController->SetTrackEnable(0, false);
		m_pAnimationController->SetTrackEnable(1, false);

		m_nAnimationState = PlayerAnimationState::Player_State_Melee;
		UnableAnimationTrack(2);
		m_pAnimationController->SetTrackEnable(2, true);
		m_pAnimationController->SetTrackAnimationSet(2, Player_Anim_Index_MeleeOneHand);
		m_pAnimationController->SetTrackWeight(2, 1);
		break;
	}

	m_Acceleration = 1.0f;

	// 약간의 전진 -> 몬스터와의 거리에 따라 조절 필요
	/*XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR addVelocity = look * 50.0f;
	XMFLOAT3 xmf3AddVelocity;
	XMStoreFloat3(&xmf3AddVelocity, addVelocity);
	m_pBody->AddVelocity(xmf3AddVelocity);*/
}

void Player::OnHit()
{
}

void Player::OnDeath()
{
}

void Player::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	Object::ApplyDamage(power, xmf3DamageDirection);

	m_pAnimationController->SetTrackEnable(0, false);
	m_pAnimationController->SetTrackEnable(1, false);

	if (m_HP > 0)
	{
		m_nAnimationState = PlayerAnimationState::Player_State_Hit;
		UnableAnimationTrack(2);
		m_pAnimationController->SetTrackEnable(2, true);
		m_pAnimationController->SetTrackAnimationSet(2, Player_Anim_Index_GetHit);
		m_pAnimationController->SetTrackWeight(2, 1);
	}
	else
	{
		m_nAnimationState = PlayerAnimationState::Player_State_Death;
		UnableAnimationTrack(2);
		m_pAnimationController->SetTrackEnable(2, true);
		m_pAnimationController->SetTrackAnimationSet(2, Player_Anim_Index_Death);
		m_pAnimationController->SetTrackWeight(2, 1);
	}
	
}

void Player::DoLanding()
{
	m_bIsFalling = false;
	m_bCanDoubleJump = true;
	m_MaxSpeedXZ = 100.f;
	m_CharacterFriction = 350.0f;
	

	switch (m_nAnimationState)
	{
	//case Player_State_Idle:
	//	break;
	//case Player_State_Jump:
	//	break;
	case Player_State_Falling:
		m_pAnimationController->SetTrackAnimationSet(2, Player_Anim_Index_JumpDown);
		m_nAnimationState = PlayerAnimationState::Player_State_Land;
		break;
	//case Player_State_Land:
	//	break;
	case Player_State_Melee:
		break;
	default:
		m_Acceleration = 500.0f;
		break;
	}

}

void Player::UpdateAnimationTrack(float elapsedTime)
{
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
		m_pAnimationController->SetTrackAnimationSet(2, Player_Anim_Index_JumpUp);
		// 낙하 애니메이션과 블랜딩
		if (m_pAnimationController->GetTrackRate(2) > 0.8f)
		{
			m_pAnimationController->SetTrackEnable(3, true);

			float weight = (m_pAnimationController->GetTrackRate(2) - 0.8f) * 5;
			m_pAnimationController->SetTrackWeight(2, 1 - weight);
			m_pAnimationController->SetTrackWeight(3, weight);
		}
		// jump_up 애니메이션 종료
		if (m_pAnimationController->GetTrackOver(2))
		{
			UnableAnimationTrack(2);

			m_pAnimationController->SetTrackWeight(3, 1);
			m_nAnimationState = PlayerAnimationState::Player_State_Falling;
		}
	}
		break;

	case Player_State_Falling:
		break;
	case Player_State_Land:
	{
		m_pAnimationController->SetTrackEnable(2, true);

		float trackRate = m_pAnimationController->GetTrackRate(2);
		if (trackRate > 0.2f)
		{
			m_pAnimationController->SetTrackEnable(3, false);

			float weight = (trackRate - 0.2f) * 1.25f;
			m_pAnimationController->SetTrackWeight(2, 1 - weight);

			BlendWithIdleMovement(weight);
		}
		else
		{
			float weight = trackRate * 5;
			m_pAnimationController->SetTrackWeight(2, weight);
			m_pAnimationController->SetTrackWeight(3, 1 - weight);
		}

		if (m_pAnimationController->GetTrackOver(2))
		{
			UnableAnimationTrack(2);

			m_nAnimationState = PlayerAnimationState::Player_State_Idle;
		}
	}
	break;

	case Player_State_Melee:
	{
		float trackRate = m_pAnimationController->GetTrackRate(2);
		if (trackRate > 0.6f)
		{
			m_pWeapon->SetActive(false);

			float weight = (trackRate - 0.6f) * 2.5f;
			m_pAnimationController->SetTrackWeight(2, 1 - weight);

			BlendWithIdleMovement(weight);
		}
		else if(trackRate > 0.2f)
		{
			m_pWeapon->SetActive(true);
		}

		if (m_pAnimationController->GetTrackOver(2))
		{
			UnableAnimationTrack(2);

			m_nAnimationState = PlayerAnimationState::Player_State_Idle;
		}
	}
		break;

	case Player_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(2);
		if (trackRate > 0.6f)
		{
			float weight = (trackRate - 0.6f) * 2.5f;
			m_pAnimationController->SetTrackWeight(2, 1 - weight);

			BlendWithIdleMovement(weight);
		}
		if (m_pAnimationController->GetTrackOver(2))
		{
			UnableAnimationTrack(2);

			m_nAnimationState = PlayerAnimationState::Player_State_Idle;
		}
	}
		break;

	case Player_State_Death:
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		m_pBody->SetAcceleration(XMFLOAT3(0, xmf3Accel.y, 0));

		if (m_pAnimationController->GetTrackOver(2))
		{
			m_ElapsedDestroyTime += elapsedTime;
			if (m_ElapsedDestroyTime > m_DestroyTime)
				m_bIsAlive = false;
		}
	}
		break;

	default:
		break;
	}
}
