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
	if (dwDirection == 0)
	{
		m_xmf3Acceleration.x = 0;
		m_xmf3Acceleration.z = 0;
	}

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

	XMVECTOR deltaAccel = direction * m_Acceleration;
	//XMFLOAT3 xmf3deltaAccel;
	//XMStoreFloat3(&xmf3deltaAccel, deltaAccel);
	XMFLOAT3 xmf3deltaAccelXZ;
	XMStoreFloat3(&xmf3deltaAccelXZ, deltaAccel);
	m_xmf3Acceleration.x = xmf3deltaAccelXZ.x;
	//m_xmf3Acceleration.y = m_xmf3Acceleration.y;
	m_xmf3Acceleration.z = xmf3deltaAccelXZ.z;

	if (m_pAnimationController->GetTrackEnable(2))
		return;
	else
	{
		m_pAnimationController->SetTrackEnable(0, true);
		m_pAnimationController->SetTrackEnable(1, true);
	}

	float tmp = 0;
	XMVECTOR velocity = XMLoadFloat3(&m_xmf3Velocity);
	tmp = XMVectorGetX(XMVector3Length(velocity)) / m_MaxSpeedXZ;

	m_pAnimationController->SetTrackWeight(0, 1 - tmp);
	m_pAnimationController->SetTrackWeight(1, tmp);
}

void Player::Jump()
{
	if (!m_bIsFalling)
	{
		m_bIsFalling = true;
		//XMVECTOR deltaAccel = XMLoadFloat3(&m_xmf3Acceleration) + (XMLoadFloat3(&m_xmf3Up) * 100.0f);
		//XMStoreFloat3(&m_xmf3Acceleration, deltaAccel);

		XMVECTOR deltaVelocity = XMLoadFloat3(&m_xmf3Velocity) + (XMLoadFloat3(&m_xmf3Up) * m_JumpSpeed);
		XMStoreFloat3(&m_xmf3Velocity, deltaVelocity);
	}
}

void Player::Attack()
{
	m_pAnimationController->SetTrackAnimationSet(2, 13);
	m_pAnimationController->SetTrackEnable(2, true);
	m_pAnimationController->SetTrackPosition(2, 0);
	m_pAnimationController->SetTrackEnable(0, false);
	m_pAnimationController->SetTrackEnable(1, false);
	m_pAnimationController->m_vpAnimationTracks[2]->SetType(ANIMATION_TYPE_ONCE);
}

void Player::OnHit()
{
}

void Player::OnDeath()
{
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
