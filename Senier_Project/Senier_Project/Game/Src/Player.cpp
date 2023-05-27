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

	std::shared_ptr<AnimationCallbackHandler> pHandler = std::make_shared<AttackCallbackHandler>();
	m_pAnimationController->SetAnimationCallbackHandler(0, pHandler);

	m_pAnimationController->SetCallbackKeys(0, 2);

	return true;
}

void Player::Update(float elapsedTime)
{
	Character::Update(elapsedTime);

	RotateToMove(elapsedTime);
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

	if (pKeybuffer[VK_SPACE] & 0xF0) Jump();

	if (pKeybuffer[VK_LBUTTON] & 0xF0)	Attack();
}

int tmpCnt = 0;

void Player::KeyDownEvent(WPARAM wParam)
{
	if (wParam == 'K')
	{
		tmpCnt = (tmpCnt + 1) % 17;
		m_pAnimationController->SetTrackAnimationSet(0, tmpCnt);
	}
}

void Player::Move(DWORD dwDirection)
{
	//if (!m_pAnimationController->GetTrackEnable(2))
	//	m_pAnimationController->SetTrackEnable(0, true);

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

	// move 애니메이션 블랜딩

	if (m_pAnimationController->GetTrackEnable(2))
		return;
	else
	{
		m_pAnimationController->SetTrackEnable(0, true);
		m_pAnimationController->SetTrackEnable(1, true);
	}

	float tmp = 0;
	XMVECTOR velocity = XMLoadFloat3(&m_pBody->GetVelocity());
	tmp = XMVectorGetX(XMVector3Length(velocity)) / m_MaxSpeedXZ;
	
	if (tmp < 0.1f)
		tmp = 0;
	else if (tmp > 1)
		tmp = 1;
	m_pAnimationController->SetTrackWeight(0, 1 - tmp);
	m_pAnimationController->SetTrackWeight(1, tmp);
}

void Player::Jump()
{
	if (!m_bIsFalling)
	{
		XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
		float length = xmf3Velocity.y;
		if (length > 5)
			return;

		//m_bIsFalling = true;
		//XMVECTOR deltaAccel = XMLoadFloat3(&m_xmf3Acceleration) + (XMLoadFloat3(&m_xmf3Up) * 100.0f);
		//XMStoreFloat3(&m_xmf3Acceleration, deltaAccel);

		XMVECTOR deltaVelocity = XMLoadFloat3(&m_pBody->GetVelocity()) + (XMLoadFloat3(&m_xmf3Up) * m_JumpSpeed);
		XMFLOAT3 xmf3DeltaVel;
		XMStoreFloat3(&xmf3DeltaVel, deltaVelocity);
		m_pBody->SetVelocity(xmf3DeltaVel);
	}
}

void Player::Attack()
{
	/*m_pAnimationController->SetTrackAnimationSet(2, 13);
	m_pAnimationController->SetTrackEnable(2, true);
	m_pAnimationController->SetTrackPosition(2, 0);
	m_pAnimationController->SetTrackEnable(0, false);
	m_pAnimationController->SetTrackEnable(1, false);
	m_pAnimationController->m_vpAnimationTracks[2]->SetType(ANIMATION_TYPE_ONCE);*/
}

void Player::OnHit()
{
}

void Player::OnDeath()
{
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

// ====================================================================

void AttackCallbackHandler::HandleCallback(void* pCallbackData, float TrackPosition)
{
	//_TCHAR* pDataName = (_TCHAR*)pCallbackData;
#ifdef _WITH_DEBUG_CALLBACK_DATA
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("%s(%f)\n"), pWavName, fTrackPosition);
	OutputDebugString(pstrDebug);
#endif

	Weapon* pWeapon = (Weapon*)pCallbackData;
	pWeapon->SetActive(true);
}
