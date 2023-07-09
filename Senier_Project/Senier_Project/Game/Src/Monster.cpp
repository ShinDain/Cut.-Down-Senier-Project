#include "../Header/Monster.h"

#define ZOMBIE_MAXSPEED 50.0f

#define MONSTER_IDLE_TRACK 0
#define MONSTER_MOVE_TRACK 1
#define MONSTER_LOOP_TRACK 2
#define MONSTER_ONCE_TRACK_1 3
#define MONSTER_ONCE_TRACK_2 4

// =========================================
// 몬스터 클래스
// =========================================
void Monster::InitAnimationTrack(float animationSpeed)
{
	// 애니메이션 기본 재생 속도
	m_AnimationSpeed = animationSpeed;
	for (int i = 0; i < MONSTER_TRACK_CNT; ++i)
	{
		m_pAnimationController->SetTrackEnable(i, false);
		m_pAnimationController->SetTrackSpeed(i, m_AnimationSpeed);
	}
	m_pAnimationController->m_vpAnimationTracks[MONSTER_ONCE_TRACK_1]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->m_vpAnimationTracks[MONSTER_ONCE_TRACK_2]->SetType(ANIMATION_TYPE_ONCE);
}

void Monster::Update(float elapsedTime)
{
	Character::Update(elapsedTime);

	m_pBody->SetAcceleration(m_xmf3MonsterMovement);

	// state에 따른 행동
	StateAction(elapsedTime);
}

void Monster::UpdateAnimationTrack(float elapsedTime)
{
	BlendWithIdleMovement(1);
}
void Monster::MonsterMove(XMFLOAT3 xmf3Direction)
{
	XMVECTOR monsterMovement = XMLoadFloat3(&xmf3Direction);
	monsterMovement *= m_DefaultAccel;

	XMStoreFloat3(&m_xmf3MonsterMovement, monsterMovement);
}

void Monster::RotateToPlayer()
{
	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);

	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);
	XMFLOAT3 xmf3TargetLook;
	XMStoreFloat3(&xmf3TargetLook, targetPosition - myPosition);

	RotateToTargetLook(0.0f, xmf3TargetLook, 1);
}

void Monster::Patrol()
{
	if (!g_pPlayer->GetIsAlive())
		return;

	// 패트롤, 왔다 갔다


	// 탐지 위치 시점 앞으로 이동
	// 플레이어 탐지 시 추적 상태로 변경
	XMVECTOR searchPos = XMLoadFloat3(&m_xmf3Position);
	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	searchPos = searchPos + (m_SearchRadius * 0.8f) * look;
	XMFLOAT3 xmf3SearchPos;
	XMStoreFloat3(&xmf3SearchPos, searchPos);

	BoundingSphere boundSphere;
	boundSphere.Center = xmf3SearchPos;
	boundSphere.Radius = m_SearchRadius;
	BoundingOrientedBox playerCollider = std::static_pointer_cast<ColliderBox>(g_pPlayer->GetCollider())->GetOBB();
	if (boundSphere.Intersects(playerCollider))
		m_bFindPlayer = true;
}

void Monster::Trace()
{
	// 맹목적으로 플레이어를 추적한다.
	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);

	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);

	XMVECTOR accelDir = targetPosition - myPosition;
	accelDir = XMVector3Normalize(accelDir);
	XMVECTOR deltaAccelXZ = accelDir * m_Acceleration;
	XMFLOAT3 xmf3DeltaAccelXZ;
	XMStoreFloat3(&xmf3DeltaAccelXZ, deltaAccelXZ);

	XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
	xmf3Accel.x = xmf3DeltaAccelXZ.x;
	xmf3Accel.z = xmf3DeltaAccelXZ.z;
	m_pBody->SetAcceleration(xmf3Accel);
}

void Monster::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, UINT nHitAnimIdx, UINT nDeathAnimIdx)
{
	if (m_State == MonsterState::Monster_State_Death || m_bInvincible)
		return;

	Character::ApplyDamage(power, xmf3DamageDirection);

	if (m_HP <= 0)
		m_bSuperArmor = false;
	if (m_bSuperArmor)
		return;

	UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
	if (m_HP > 0)
	{
		m_State = MonsterState::Monster_State_Hit;
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, nHitAnimIdx);
		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);
	}
	else
	{
		m_pCollider->SetIsActive(false);

		//Cutting(XMFLOAT3(1, 0, 0));
		m_State = MonsterState::Monster_State_Death;
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, nDeathAnimIdx);
		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);
	}
}

void Monster::CreateAttackSphere(float range, float radius, float damage)
{
	if (!g_pPlayer->GetIsAlive())
		return;

	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	XMVECTOR attackPosition = myPosition + (l * range);

	BoundingSphere attackSphere;
	XMStoreFloat3(&attackSphere.Center, attackPosition);
	attackSphere.Center.y = 5.0f;
	attackSphere.Radius = radius;
	BoundingOrientedBox playerCollider = std::static_pointer_cast<ColliderBox>(g_pPlayer->GetCollider())->GetOBB();
	if (attackSphere.Intersects(playerCollider))
		g_pPlayer->ApplyDamage(damage, m_xmf3Look);
}

/////////////////////////////
// 일반 좀비
/////////////////////////////
Zombie::Zombie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

Zombie::~Zombie()
{
	Destroy();
}

bool Zombie::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	// 애니메이션 트랙 초기화
	InitAnimationTrack(1.5f);

	// 애니메이션 트랙 초기화
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, Zombie_Anim_Index_Idle);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Zombie_Anim_Index_Run);

	// 파괴 타이머 초기화
	m_DestroyTime = 3.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 10.0f;
	m_AttackRange = 15.0f;
	m_AttackRadius = 7.0f;

	// 체력 초기화
	m_MaxHP = 100.0f;
	m_HP = 100.0f;


	m_DefaultAccel = 550.0f;
	m_DefaultMaxSpeedXZ = 100.0f;
	m_DefaultFriction = 400.0f;


	return true;
}

void Zombie::UpdateAnimationTrack(float elapsedTime)
{
	switch (m_State)
	{
	case Monster::Monster_State_Idle:
	case Monster::Monster_State_Trace:
		BlendWithIdleMovement(1);
		break;
	case Monster::Monster_State_Attack1:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.3f && trackRate < 0.4f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 3;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.3f)
		{
			RotateToPlayer();
		}
		else if (trackRate > 0.6f)
		{
			m_bSuperArmor = false;
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.6f, 2.5f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
		break;
	case Monster::Monster_State_Attack2:
		break;
	case Monster::Monster_State_Attack3:
		break;
	case Monster::Monster_State_Hit:
	{	
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.5f, 2.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
		break;
	case Monster::Monster_State_Death:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 1.0f);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);

		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			m_bDestroying = true;
		}
	}
	break;
	case Monster::Monster_State_Special1:
		break;
	case Monster::Monster_State_Special2:
		break;
	case Monster::Monster_State_Special3:
		break;
	default:
		break;
	}
}

void Zombie::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	int nAttackAnim = rand() % 2;

	UINT hitAnimIdx = Zombie_Anim_Index_Hit1 + nAttackAnim;
	UINT deathAnimIdx = Zombie_Anim_Index_FallingBack + nAttackAnim;

	Monster::ApplyDamage(power, xmf3DamageDirection, hitAnimIdx, deathAnimIdx);
}

void Zombie::StateAction(float elapsedTime)
{
	switch (m_State)
	{
	case Monster_State_Idle:
		Patrol();
		if (m_bFindPlayer) m_State = Monster_State_Trace;
		break;
	case Monster_State_Trace:
		Trace();
		break;
	default:
		break;
	}
}

void Zombie::Trace()
{
	Monster::Trace();

	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);

	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);
	XMVECTOR accelDir = targetPosition - myPosition;
	if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 1.1f)
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		xmf3Accel.x = 0;
		xmf3Accel.z = 0;
		m_pBody->SetAcceleration(xmf3Accel);

		Attack1();
		return;
	}
}

void Zombie::Attack1()
{
	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	if (XMVectorGetX(XMVector3Length(myPosition - playerPosition)) > m_AttackRange * 3)
	{
		m_State = Monster_State_Trace;
		return;
	}

	switch (m_State)
	{
		/*case Zombie_State_Idle:
			break;
		case Zombie_State_Jump:
			break;
		case Zombie_State_Falling:
			break;
		case Zombie_State_Land:
			break;*/
	case Monster_State_Attack1:
		break;
	default:
		//int attackAnimIdx = rand() % 2;
		int attackAnimIdx = 0;

		m_State = MonsterState::Monster_State_Attack1;
		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Zombie_Anim_Index_Attack1 + attackAnimIdx);

		break;
	}

}

/////////////////////////////
// 고급 좀비
/////////////////////////////
HighZombie::HighZombie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

HighZombie::~HighZombie()
{
	Destroy();
}

bool HighZombie::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	InitAnimationTrack(3.0f);
	int idleAnimIdx = rand() % 2;

	// 애니메이션 트랙 초기화
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, HighZombie_Anim_Index_Idle1 + idleAnimIdx);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, HighZombie_Anim_Index_Run);

	// 파괴 타이머 초기화
	m_DestroyTime = 3.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 10.0f;
	m_AttackRange = 16.0f;
	m_AttackRadius = 5.0f;

	// 체력 초기화
	m_MaxHP = 200.0f;
	m_HP = 200.0f;

	m_DefaultAccel = 500.0f;
	m_DefaultMaxSpeedXZ = 100.0f;
	m_DefaultFriction = 400.0f;

	return true;
}

void HighZombie::UpdateAnimationTrack(float elapsedTime)
{
	switch (m_State)
	{
	case Monster::Monster_State_Idle:
	case Monster::Monster_State_Trace:
		BlendWithIdleMovement(1);
		break;
	case Monster::Monster_State_Attack1:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.3f && trackRate < 0.4f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 4;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.3f)
		{
			RotateToPlayer();
		}
		else if (trackRate > 0.6f)
		{
			m_bSuperArmor = false;
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Attack2:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.2f && trackRate < 0.3f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 4;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}
		if (trackRate > 0.5f && trackRate < 0.6f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 4;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.3f)
		{
			RotateToPlayer();
		}
		else if (trackRate > 0.8f)
		{
			m_bSuperArmor = false;
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
		break;
	case Monster::Monster_State_Attack3:
		break;
	case Monster::Monster_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.5f, 2.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Death:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);

		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			m_bDestroying = true;
		}
	}
	break;
	case Monster::Monster_State_Special1:
		break;
	case Monster::Monster_State_Special2:
		break;
	case Monster::Monster_State_Special3:
		break;
	default:
		break;
	}
}

void HighZombie::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	int nHitAnim = rand() % 2;

	UINT hitAnimIdx = HighZombie_Anim_Index_Hit;
	UINT deathAnimIdx = HighZombie_Anim_Index_Death1 + nHitAnim;

	Monster::ApplyDamage(power, xmf3DamageDirection, hitAnimIdx, deathAnimIdx);
}

void HighZombie::StateAction(float elapsedTime)
{
	switch (m_State)
	{
	case Monster_State_Idle:
		Patrol();
		if (m_bFindPlayer) m_State = Monster_State_Trace;
		break;
	case Monster_State_Trace:
		Trace();
		break;
	default:
		break;
	}
}

void HighZombie::Trace()
{
	Monster::Trace();

	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);

	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);
	XMVECTOR accelDir = targetPosition - myPosition;
	if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 1.1f)
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		xmf3Accel.x = 0;
		xmf3Accel.z = 0;
		m_pBody->SetAcceleration(xmf3Accel);

		Attack1();
		return;
	}
}

void HighZombie::Attack1()
{
	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	if (XMVectorGetX(XMVector3Length(myPosition - playerPosition)) > m_AttackRange * 3)
	{
		m_State = Monster_State_Trace;
		return;
	}

	switch (m_State)
	{
		/*case Zombie_State_Idle:
			break;
		case Zombie_State_Jump:
			break;
		case Zombie_State_Falling:
			break;
		case Zombie_State_Land:
			break;*/
	case Monster_State_Attack1:
		break;
	default:
	{
		int attackAnimIdx = rand() % 2;

		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, HighZombie_Anim_Index_Attack1 + attackAnimIdx);

		switch (attackAnimIdx)
		{
		case 0:
			m_State = MonsterState::Monster_State_Attack1;
			break;

		case 1:
			m_State = MonsterState::Monster_State_Attack2;
			break;
		}
	}
		break;
	}
}

/////////////////////////////
// 사마귀
/////////////////////////////
Scavenger::Scavenger(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

Scavenger::~Scavenger()
{
	Destroy();
}

bool Scavenger::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	InitAnimationTrack(5.0f);
	int idleAnimIdx = rand() % 2;

	// 애니메이션 트랙 초기화
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, Scavenger_Anim_Index_Idle1 + idleAnimIdx);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Scavenger_Anim_Index_Walk);

	// 파괴 타이머 초기화
	m_DestroyTime = 3.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 10.0f;
	m_AttackRange = 15.0f;
	m_AttackRadius = 3.0f;

	// 체력 초기화
	m_MaxHP = 50.f;
	m_HP = 50.f;

	m_DefaultAccel = 550.0f;
	m_DefaultMaxSpeedXZ = 100.0f;
	m_DefaultFriction = 500.0f;

	// 일단은 100의 속도로 사용
	// 애니메이션이 걷기 밖에 없기에
	// 200의 이동속도를 표현하기; 위해선 5배속으로 애니메이션 재생해야함 


	return true;
}

void Scavenger::UpdateAnimationTrack(float elapsedTime)
{
	switch (m_State)
	{
	case Monster::Monster_State_Idle:
	case Monster::Monster_State_Trace:
		BlendWithIdleMovement(1);
		break;
	case Monster::Monster_State_Attack1:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.5f && trackRate < 0.6f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 3;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.4f)
		{
			RotateToPlayer();
		}
		else if (trackRate > 0.6f)
		{
			m_bSuperArmor = false;
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Attack2:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.4f && trackRate < 0.5f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 7;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			
		}
		else if (trackRate > 0.5f && trackRate < 0.6f)
		{
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.3f)
		{
			RotateToPlayer();
		}
		else if (trackRate > 0.6f)
		{
			m_bSuperArmor = false;
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Attack3:
		break;
	case Monster::Monster_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.5f, 2.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Death:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);

		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			m_bDestroying = true;
		}
	}
	break;
	case Monster::Monster_State_Special1:
		break;
	case Monster::Monster_State_Special2:
		break;
	case Monster::Monster_State_Special3:
		break;
	default:
		break;
	}
}

void Scavenger::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	int nHitAnim = rand() % 2;

	UINT hitAnimIdx = Scavenger_Anim_Index_Hit1 + nHitAnim;
	UINT deathAnimIdx = Scavenger_Anim_Index_Death1 + nHitAnim;

	Monster::ApplyDamage(power, xmf3DamageDirection, hitAnimIdx, deathAnimIdx);
}

void Scavenger::StateAction(float elapsedTime)
{
	switch (m_State)
	{
	case Monster_State_Idle:
		Patrol();
		if (m_bFindPlayer) m_State = Monster_State_Trace;
		break;
	case Monster_State_Trace:
		Trace();
		break;
	default:
		break;
	}
}

void Scavenger::Trace()
{
	Monster::Trace();

	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);

	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);
	XMVECTOR accelDir = targetPosition - myPosition;
	if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 1.1f)
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		xmf3Accel.x = 0;
		xmf3Accel.z = 0;
		m_pBody->SetAcceleration(xmf3Accel);

		Attack1();
		return;
	}
}

void Scavenger::Attack1()
{
	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	if (XMVectorGetX(XMVector3Length(myPosition - playerPosition)) > m_AttackRange * 3)
	{
		m_State = Monster_State_Trace;
		return;
	}

	switch (m_State)
	{
	//case Monster_State_Idle:
	//	break;
	case Monster_State_Attack1:
		break;
	default:
	{
		int attackAnimIdx = rand() % 2;

		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Scavenger_Anim_Index_Attack1 + attackAnimIdx);

		switch (attackAnimIdx)
		{
		case 0:
			m_State = MonsterState::Monster_State_Attack1;
			break;

		case 1:
			m_State = MonsterState::Monster_State_Attack2;
			break;
		}
	}
	break;
	}
}

/////////////////////////////
// 거대 구울
/////////////////////////////
Ghoul::Ghoul(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

Ghoul::~Ghoul()
{
	Destroy();
}

bool Ghoul::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	InitAnimationTrack(1);

	// 애니메이션 트랙 초기화
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, Ghoul_Anim_Index_Idle);
	//m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Ghoul_Anim_Index_Walk);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Ghoul_Anim_Index_Run);

	// 파괴 타이머 초기화
	m_DestroyTime = 5.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 10.0f;
	m_AttackRange = 32.0f;
	m_AttackRadius = 10.0f;

	// 체력 초기화
	m_MaxHP = 300.0f;
	m_HP = 300.0f;

	// 
	m_DefaultAccel = 500.0f;
	m_DefaultMaxSpeedXZ = 100.0f;

	return true;
}

void Ghoul::Animate(float elapsedTime)
{
	float eTime = elapsedTime;

	// 쳐다보고 있는 동안 멈춘 상태로
	//if (m_bVisible)
	//{
	//	eTime = 0.0f;
	//}

	if (m_pAnimationController)
		m_pAnimationController->AdvanceTime(eTime, this);

	if (m_pSibling) { m_pSibling->Animate(eTime); }
	if (m_pChild) { m_pChild->Animate(eTime); }
}

void Ghoul::UpdateAnimationTrack(float elapsedTime)
{
	//if (m_bVisible)
	//{
	//	//m_AnimationSpeed = 0;
	//	m_DefaultMaxSpeedXZ = 100.0f;
	//	m_DefaultAccel = 0.0f;
	//}
	//else
	//{
	//	//m_AnimationSpeed = 1;
	//	m_DefaultMaxSpeedXZ = 100.0f;
	//	m_DefaultAccel = 500.0f;
	//}

	switch (m_State)
	{
	case Monster::Monster_State_Idle:
	case Monster::Monster_State_Trace:
		BlendWithIdleMovement(1);
		break;
	case Monster::Monster_State_Attack1:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.4f && trackRate < 0.5f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 3;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}
		if (trackRate > 0.65f && trackRate < 0.75f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 1;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.4f)
		{
			RotateToPlayer();
		}
		else if (trackRate > 0.86f)
		{
			m_bSuperArmor = false;
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Attack2:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.4f && trackRate < 0.5f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 1;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);

		}
		else if (trackRate > 0.5f && trackRate < 0.6f)
		{
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.4f)
		{
			RotateToPlayer();
		}
		else if (trackRate > 0.6f)
		{
			m_bSuperArmor = false;
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Attack3:
		break;
	case Monster::Monster_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.5f, 2.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Death:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);

		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			m_bDestroying = true;
		}
	}
	break;
	case Monster::Monster_State_Special1:
		break;
	case Monster::Monster_State_Special2:
		break;
	case Monster::Monster_State_Special3:
		break;
	default:
		break;
	}
}

void Ghoul::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	UINT hitAnimIdx = Ghoul_Anim_Index_Death;
	UINT deathAnimIdx = Ghoul_Anim_Index_Death;

	Monster::ApplyDamage(power, xmf3DamageDirection, hitAnimIdx, deathAnimIdx);
}

void Ghoul::StateAction(float elapsedTime)
{
	switch (m_State)
	{
	case Monster_State_Idle:
		Patrol();
		if (m_bFindPlayer) m_State = Monster_State_Trace;
		break;
	case Monster_State_Trace:
		Trace();
		break;
	default:
		break;
	}
}

void Ghoul::Trace()
{
	Monster::Trace();

	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);

	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);
	XMVECTOR accelDir = targetPosition - myPosition;
	if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 1.1f)
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		xmf3Accel.x = 0;
		xmf3Accel.z = 0;
		m_pBody->SetAcceleration(xmf3Accel);

		Attack1();
		return;
	}
}

void Ghoul::Attack1()
{
	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	if (XMVectorGetX(XMVector3Length(myPosition - playerPosition)) > m_AttackRange * 3)
	{
		m_State = Monster_State_Trace;
		return;
	}

	switch (m_State)
	{
		//case Monster_State_Idle:
		//	break;
	case Monster_State_Attack1:
		break;
	default:
	{
		int attackAnimIdx = rand() % 2;

		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Ghoul_Anim_Index_Attack1 + attackAnimIdx);

		switch (attackAnimIdx)
		{
		case 0:
			m_State = MonsterState::Monster_State_Attack1;
			break;

		case 1:
			m_State = MonsterState::Monster_State_Attack2;
			break;
		}
	}
	break;
	}
}

/////////////////////////////
// 사이버 트윈스
/////////////////////////////
CyberTwins::CyberTwins(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

CyberTwins::~CyberTwins()
{
	Destroy();
}

bool CyberTwins::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	InitAnimationTrack(1.5f);

	// 애니메이션 트랙 초기화
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, CyberTwins_Anim_Index_Idle);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, CyberTwins_Anim_Index_Run);

	// 파괴 타이머 초기화
	m_DestroyTime = 5.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 10.0f;
	m_AttackRange = 27.0f;
	m_AttackRadius = 10.0f;

	// 체력 초기화
	m_MaxHP = 500.0f;
	m_HP = 500.0f;

	// 
	m_DefaultAccel = 500.0f;
	m_DefaultMaxSpeedXZ = 100.0f;

	return true;
}

void CyberTwins::UpdateAnimationTrack(float elapsedTime)
{
	switch (m_State)
	{
	case Monster::Monster_State_Idle:
	case Monster::Monster_State_Trace:
		BlendWithIdleMovement(1);
		break;
	case Monster::Monster_State_Attack1:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.5f && trackRate < 0.6f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 3;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}


		if (trackRate < 0.4f)
		{
			RotateToPlayer();
		}
		else if (trackRate > 0.8f)
		{
			m_bSuperArmor = false;
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Attack2:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.4f && trackRate < 0.5f)
		{
			m_bSuperArmor = true;

			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 1;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);

		}
		else if (trackRate > 0.5f && trackRate < 0.6f)
		{
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.4f)
		{
			RotateToPlayer();
		}
		else if (trackRate > 0.6f)
		{
			m_bSuperArmor = false;
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Attack3:
		break;
	case Monster::Monster_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.5f, 2.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Death:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);

		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			m_bDestroying = true;
		}
	}
	break;
	case Monster::Monster_State_Special1:
		break;
	case Monster::Monster_State_Special2:
		break;
	case Monster::Monster_State_Special3:
		break;
	default:
		break;
	}
}

void CyberTwins::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	UINT hitAnimIdx = CyberTwins_Anim_Index_Death;
	UINT deathAnimIdx = CyberTwins_Anim_Index_Death;

	Monster::ApplyDamage(power, xmf3DamageDirection, hitAnimIdx, deathAnimIdx);
}

void CyberTwins::StateAction(float elapsedTime)
{
	switch (m_State)
	{
	case Monster_State_Idle:
		Patrol();
		if (m_bFindPlayer) m_State = Monster_State_Trace;
		break;
	case Monster_State_Trace:
		Trace();
		break;
	default:
		break;
	}
}

void CyberTwins::Trace()
{
	Monster::Trace();

	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);

	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);
	XMVECTOR accelDir = targetPosition - myPosition;
	if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 1.1f)
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		xmf3Accel.x = 0;
		xmf3Accel.z = 0;
		m_pBody->SetAcceleration(xmf3Accel);

		Attack1();
		return;
	}
}

void CyberTwins::Attack1()
{
	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	if (XMVectorGetX(XMVector3Length(myPosition - playerPosition)) > m_AttackRange * 3)
	{
		m_State = Monster_State_Trace;
		return;
	}

	switch (m_State)
	{
		//case Monster_State_Idle:
		//	break;
	case Monster_State_Attack1:
		break;
	default:
	{
		//int attackAnimIdx = rand() % 2;
		int attackAnimIdx = 0;

		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, CyberTwins_Anim_Index_Attack1 + attackAnimIdx);

		switch (attackAnimIdx)
		{
		case 0:
			m_State = MonsterState::Monster_State_Attack1;
			break;

		case 1:
			m_State = MonsterState::Monster_State_Attack2;
			break;
		}
	}
	break;
	}
}

/////////////////////////////
// 강령 술사
/////////////////////////////
Necromancer::Necromancer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);
}

Necromancer::~Necromancer()
{
	Destroy();
}

bool Necromancer::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	InitAnimationTrack(3.0f);
	int idleAnimIdx = rand() % 2;

	// 애니메이션 트랙 초기화
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, Necromancer_Anim_Index_Idle1 + idleAnimIdx);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Necromancer_Anim_Index_Walk);

	// 파괴 타이머 초기화
	m_DestroyTime = 5.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 10.0f;
	m_AttackRange = 27.0f;
	m_AttackRadius = 10.0f;

	// 체력 초기화
	m_MaxHP = 500.0f;
	m_HP = 500.0f;

	// 
	m_DefaultAccel = 500.0f;
	m_DefaultMaxSpeedXZ = 50.0f;

	m_bSuperArmor = true;

	return true;
}

void Necromancer::UpdateAnimationTrack(float elapsedTime)
{
	switch (m_State)
	{
	case Monster::Monster_State_Idle:
	case Monster::Monster_State_Trace:
		BlendWithIdleMovement(1);
		break;
	case Monster::Monster_State_Attack1:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.4f && trackRate < 0.5f)
		{
			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 3;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}


		if (trackRate < 0.65f)
		{
			RotateToPlayer();
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Attack2:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.4f && trackRate < 0.5f)
		{
			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 1;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);

		}
		else if (trackRate > 0.5f && trackRate < 0.6f)
		{
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.4f)
		{
			RotateToPlayer();
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Attack3:
		break;
	case Monster::Monster_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.5f, 2.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
		}
	}
	break;
	case Monster::Monster_State_Death:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);

		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			m_bDestroying = true;
		}
	}
	break;
	case Monster::Monster_State_Special1:
		break;
	case Monster::Monster_State_Special2:
		break;
	case Monster::Monster_State_Special3:
		break;
	default:
		break;
	}
}

void Necromancer::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	UINT hitAnimIdx = Necromancer_Anim_Index_Hit;
	UINT deathAnimIdx = Necromancer_Anim_Index_Death;

	Monster::ApplyDamage(power, xmf3DamageDirection, hitAnimIdx, deathAnimIdx);
}
																		
void Necromancer::StateAction(float elapsedTime)
{
	switch (m_State)
	{
	case Monster_State_Idle:
		Patrol();
		if (m_bFindPlayer) m_State = Monster_State_Trace;
		break;
	case Monster_State_Trace:
		Trace();
		break;
	default:
		break;
	}
}

void Necromancer::Trace()
{
	Monster::Trace();

	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);

	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);
	XMVECTOR accelDir = targetPosition - myPosition;
	if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 1.1f)
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		xmf3Accel.x = 0;
		xmf3Accel.z = 0;
		m_pBody->SetAcceleration(xmf3Accel);

		Attack1();
		return;
	}
}

void Necromancer::Attack1()
{
	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	if (XMVectorGetX(XMVector3Length(myPosition - playerPosition)) > m_AttackRange * 3)
	{
		m_State = Monster_State_Trace;
		return;
	}

	switch (m_State)
	{
		//case Monster_State_Idle:
		//	break;
	case Monster_State_Attack1:
		break;
	default:
	{
		//int attackAnimIdx = rand() % 2;
		int attackAnimIdx = 0;

		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Necromancer_Anim_Index_Attack1 + attackAnimIdx);

		switch (attackAnimIdx)
		{
		case 0:
			m_State = MonsterState::Monster_State_Attack1;
			break;

		case 1:
			m_State = MonsterState::Monster_State_Attack2;
			break;
		}
	}
	break;
	}
}
