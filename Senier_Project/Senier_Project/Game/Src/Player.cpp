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

	return true;
}

void Player::Update(float elapsedTime)
{
	Character::Update(elapsedTime);

	RotateToMove(elapsedTime);
	UpdateAnimationTrack();

	m_pWeapon->Intersect(m_xmf3Look);
}

void Player::Destroy()
{
	Character::Destroy();

	if(m_pWeapon) m_pWeapon->Destroy();
	m_pWeapon.reset();
}

void Player::ProcessInput(UCHAR* pKeybuffer)
{
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
	/*if (wParam == 'K')
	{
		tmpCnt = (tmpCnt + 1) % 17;
		m_pAnimationController->SetTrackAnimationSet(0, tmpCnt);
	}*/
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

	UnableAnimationTrack(3);
}

void Player::BlendWithIdleMovement(float maxWeight)
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

	m_Acceleration = 0.0f;

	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR addVelocity = look * 100.0f;
	XMFLOAT3 xmf3AddVelocity;
	XMStoreFloat3(&xmf3AddVelocity, addVelocity);
	m_pBody->AddVelocity(xmf3AddVelocity);
}

void Player::OnHit()
{
}

void Player::OnDeath()
{
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

void Player::UpdateAnimationTrack()
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
		m_pAnimationController->SetTrackEnable(0, false);
		m_pAnimationController->SetTrackEnable(1, false);

		m_pAnimationController->SetTrackAnimationSet(2, Player_Anim_Index_JumpUp);
		// 낙하 애니메이션과 블랜딩
		if (m_pAnimationController->GetTrackRate(2) > 0.8f)
		{
			m_pAnimationController->SetTrackEnable(3, true);

			float weight = (1.0f - m_pAnimationController->GetTrackRate(2)) * 5;
			if (weight > 1)
				weight = 1; 
			m_pAnimationController->SetTrackWeight(2, weight);
			m_pAnimationController->SetTrackWeight(3, 1 - weight);
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

			float weight = (1.0f - trackRate);
			if (weight > 1)
				weight = 1;
			m_pAnimationController->SetTrackWeight(2, weight);

			float moveWeight = 1 - weight;
			BlendWithIdleMovement(moveWeight);
		}
		else
		{
			m_pAnimationController->SetTrackWeight(2, trackRate * 5);
			m_pAnimationController->SetTrackWeight(3, 1 - (trackRate * 5));
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
		if (m_pAnimationController->GetTrackRate(2) > 0.6f)
		{
			m_pWeapon->SetActive(false);
		}
		else
		{
			m_pWeapon->SetActive(true);
		}
		if (m_pAnimationController->GetTrackOver(2))
		{
			m_pAnimationController->SetTrackEnable(0, true);
			m_pAnimationController->SetTrackEnable(1, true);

			UnableAnimationTrack(2);
			m_Acceleration = 500.0f;

			m_nAnimationState = PlayerAnimationState::Player_State_Idle;
		}
	}
		break;
	default:
		break;
	}
}

void Player::UnableAnimationTrack(int nAnimationTrack)
{
	// Track default화
	
	m_pAnimationController->SetTrackOver(nAnimationTrack, false);
	m_pAnimationController->SetTrackEnable(nAnimationTrack, false);
	m_pAnimationController->SetTrackRate(nAnimationTrack, 0);
	m_pAnimationController->SetTrackWeight(nAnimationTrack, 0);
	m_pAnimationController->SetTrackPosition(nAnimationTrack, 0);
}

void Player::RotateToMove(float elapsedTime)
{
	// 진행 방향
	XMVECTOR l = XMVectorSet(0, 0, 1, 0);
	XMVECTOR targetLook = XMLoadFloat3(&m_pBody->GetAcceleration());
	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR right = XMLoadFloat3(&m_xmf3Right);
	float angleBetweenLook = XMVectorGetX(XMVector3AngleBetweenVectors(targetLook, look));
	angleBetweenLook = XMConvertToDegrees(angleBetweenLook);

	if (!XMVectorGetX(XMVectorIsNaN(XMVectorReplicate(angleBetweenLook))))
	{
		if (XMVectorGetX(XMVector3Dot(targetLook, right)) < 0)
			angleBetweenLook *= -1;

		XMFLOAT3 xmf3Velcity = m_pBody->GetVelocity();
		xmf3Velcity.y = 0;
		XMVECTOR velocity = XMLoadFloat3(&xmf3Velcity);
		float velocityLength = XMVectorGetX(XMVector3Length(velocity));

		angleBetweenLook = angleBetweenLook / 5;

		// 값이 너무 커지지 않도록
		float tmp = (int)(m_xmf3Rotation.y + angleBetweenLook) % 360;

		SetRotate(XMFLOAT3(m_xmf3Rotation.x, tmp, m_xmf3Rotation.z));
	}
}
