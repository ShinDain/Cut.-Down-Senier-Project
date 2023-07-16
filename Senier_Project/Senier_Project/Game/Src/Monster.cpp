#include "../Header/Monster.h"
#include "../../DirectXRendering/Header/Scene.h"

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

void Monster::initializeState()
{
	UnableAnimationTrack(MONSTER_IDLE_TRACK);
	UnableAnimationTrack(MONSTER_MOVE_TRACK);
	UnableAnimationTrack(MONSTER_LOOP_TRACK);
	UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
	UnableAnimationTrack(MONSTER_ONCE_TRACK_2);

	m_bIsFalling = false;

	m_MaxSpeedXZ = m_DefaultMaxSpeedXZ;
	m_CharacterFriction = 350.0f;

	m_Acceleration = m_DefaultAccel;
	m_bSuperArmor = false;

	// 일반 상태로 강제 초기화
	m_State = Monster_State_Idle;
	BlendWithIdleMovement(1);
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

void Monster::StateAction(float elapsedTime)
{
	switch (m_State)
	{
	case Monster_State_Idle:
		Patrol();
		if (m_bFindPlayer) m_State = Monster_State_Trace;
		break;
	case Monster_State_Trace:
		if (m_bAttackEndLag)
		{
			m_ElapsedAttackEndDelay += elapsedTime;
			if (m_ElapsedAttackEndDelay >= m_AttackEndDelay)
			{
				m_bAttackEndLag = false;
				m_ElapsedAttackEndDelay = 0.0f;
			}
			break;
		}
		Trace();
		break;
	default:
		break;
	}
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
	BoundingOrientedBox* playerCollider = std::static_pointer_cast<ColliderBox>(g_pPlayer->GetCollider())->GetOBB().get();
	if (boundSphere.Intersects(*playerCollider))
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

	m_bFindPlayer = true;
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
	BoundingOrientedBox* playerCollider = std::static_pointer_cast<ColliderBox>(g_pPlayer->GetCollider())->GetOBB().get();
	if (attackSphere.Intersects(*playerCollider))
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
			m_bAttackEndLag = true;
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
	m_AttackDamage = 15.0f;
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
			m_bAttackEndLag = true;
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
			m_bAttackEndLag = true;
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

		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 3.0f);

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

	InitAnimationTrack(3.0f);
	int idleAnimIdx = rand() % 2;

	// 애니메이션 트랙 초기화
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, Scavenger_Anim_Index_Idle1 + idleAnimIdx);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Scavenger_Anim_Index_Walk);

	// 파괴 타이머 초기화
	m_DestroyTime = 3.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 10.0f;
	m_AttackRange = 10.0f;
	m_AttackRadius = 3.0f;

	// 체력 초기화
	m_MaxHP = 50.f;
	m_HP = 50.f;

	m_DefaultAccel = 500.0f;
	m_DefaultMaxSpeedXZ = 200.0f;
	m_DefaultFriction = 350.f;

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
			XMVECTOR deltaVelocity = l * 7;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, 7, m_AttackDamage);
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

			m_nAttackCnt += 1;
			if (m_nAttackCnt >= m_nRushNum)
			{
				m_bRush = true;
				m_nAttackCnt = 0;
			}

			m_bAttackEndLag = true;
		}
	}
	break;
	case Monster::Monster_State_Attack2:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_2);

		// 공격 판정
		if (trackRate < 0.8f && trackRate > 0.2f)
		{
			m_pCollider->SetIsActive(false); 
			CreateAttackSphere(0, m_xmf3ColliderExtents.x * 10, m_AttackDamage * 1.3f);
		}
		else if (trackRate > 0.8f)
		{
			m_bSuperArmor = false;
			m_pCollider->SetIsActive(true);
		}

		if (trackRate > 0.2f && m_bRush)
		{
			XMVECTOR targetPosition = XMLoadFloat3(&m_xmf3RushTargetPosition);
			XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
			XMVECTOR rushVelocity = targetPosition - myPosition;
			rushVelocity = XMVector3Normalize(rushVelocity);
			float rushSpeed = 800;
			rushVelocity *= rushSpeed;
			XMFLOAT3 xmf3RushVelocity;
			XMStoreFloat3(&xmf3RushVelocity, rushVelocity);
			m_pBody->AddVelocity(xmf3RushVelocity);
			m_bRush = false;
		}

		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_2);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_2))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_2);
			m_State = MonsterState::Monster_State_Idle;
			m_bAttackEndLag = true;
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

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);

		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			m_bDestroying = true;
		}
	}
	break;
	case Monster::Monster_State_Special1:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		// 플레이어로 회전
		RotateToPlayer();
		
		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);

		BlendAnimationToAnimation(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1, MONSTER_ONCE_TRACK_2);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_2, 1);
			m_State = MonsterState::Monster_State_Attack2;

			// 
			XMVECTOR targetPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
			XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
			XMVECTOR direction = targetPosition - myPosition;
			direction = XMVector3Normalize(direction);
			targetPosition = targetPosition + direction * 50;
			XMFLOAT3 xmf3TargetPosition;
			XMStoreFloat3(&xmf3TargetPosition, targetPosition);

			m_xmf3RushTargetPosition = xmf3TargetPosition;
			m_bSuperArmor = true;
		}
	}
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
	m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 2.0f);
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

	if (m_bRush)
	{
		if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 6.0f)
		{
			XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
			xmf3Accel.x = 0;
			xmf3Accel.z = 0;
			m_pBody->SetAcceleration(xmf3Accel);

			Attack1();
			return;
		}
	}
	else
	{
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
	
}

void Scavenger::Attack1()
{
	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	if (XMVectorGetX(XMVector3Length(myPosition - playerPosition)) > m_AttackRange * 3 && m_bRush == false)
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
		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

		
		switch (m_bRush)
		{
		case false:
			m_State = MonsterState::Monster_State_Attack1;
			m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Scavenger_Anim_Index_Attack1);
			break;

		case true:
			UnableAnimationTrack(MONSTER_ONCE_TRACK_2);
			m_State = MonsterState::Monster_State_Special1;
			m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Scavenger_Anim_Index_Rage);
			m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_2, Scavenger_Anim_Index_Attack2);
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 1.5f);
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_2, 3.0f);
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
	m_AttackDamage = 30.0f;
	m_AttackRange = 32.0f;
	m_AttackRadius = 10.0f;

	// 체력 초기화
	m_MaxHP = 300.0f;
	m_HP = 300.0f;

	// 
	m_DefaultAccel = 800.0f;
	m_DefaultMaxSpeedXZ = 200.0f;

	return true;
}

void Ghoul::Animate(float elapsedTime)
{
	float eTime = elapsedTime;

	// 쳐다보고 있는 동안 멈춘 상태로
	switch (m_State)
	{
	case Monster::Monster_State_Idle:
	case Monster::Monster_State_Trace:
	case Monster::Monster_State_Special1:
	case Monster::Monster_State_Special2:
	case Monster::Monster_State_Special3:
	case Monster::Monster_State_Hit:
		if (m_bVisible)
		{
			eTime = 0.0f;
		}
		break;

	case Monster::Monster_State_Attack1:
	case Monster::Monster_State_Attack2:
	case Monster::Monster_State_Attack3:
	case Monster::Monster_State_Death:
	default:
		break;
	}

	if (m_pAnimationController)
		m_pAnimationController->AdvanceTime(eTime, this);

	if (m_pSibling) { m_pSibling->Animate(eTime); }
	if (m_pChild) { m_pChild->Animate(eTime); }
}

void Ghoul::UpdateAnimationTrack(float elapsedTime)
{
	m_bSuperArmor = true;

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
			XMVECTOR deltaVelocity = l * 7;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}
		if (trackRate > 0.65f && trackRate < 0.75f)
		{
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

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
			m_bAttackEndLag = true;
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
			XMVECTOR deltaVelocity = l * 7;
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
			m_bAttackEndLag = true;
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

	XMFLOAT3 xmf3Velocity = m_pBody->GetVelocity();
	xmf3Velocity.x *= 0.3f;
	xmf3Velocity.y *= 0.3f;
	xmf3Velocity.z *= 0.3f;
	m_pBody->SetVelocity(xmf3Velocity);
}

void Ghoul::Trace()
{
	if (!m_bVisible)
		Monster::Trace();

	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);

	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);
	XMVECTOR accelDir = targetPosition - myPosition;


	if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 1.5f)
	{
		accelDir = XMVector3Normalize(accelDir);
		XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
		if(XMVectorGetX(XMVector3Dot(accelDir, look)) > 0)
		{ 
			XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
			xmf3Accel.x = 0;
			xmf3Accel.z = 0;
			m_pBody->SetAcceleration(xmf3Accel);

			Attack1();
		}
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
	m_AttackDamage = 20.0f;
	m_AttackRange = 27.0f;
	m_AttackRadius = 10.0f;

	// 체력 초기화
	m_MaxHP = 500.0f;
	m_HP = 500.0f;

	// 기본적으로 상시 슈퍼아머
	m_bSuperArmor = true;

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
			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 150;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}

		if (trackRate < 0.4f)
		{
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 0.5f);
			RotateToPlayer();
		}
		else if(trackRate > 0.4f && trackRate < 0.8f)
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);
		else
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 0.5f);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.9f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			BlendWithIdleMovement(1);
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
			
			m_bAttackEndLag = true;

			// 다음 패턴 
			m_nPattern = rand() % 2;
		}
	}
	break;
	case Monster::Monster_State_Attack2:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.3f && trackRate < 0.4f)
		{
			GunFire();
		}
		else if (trackRate > 0.5f && m_nMaxFireCnt > 0)
		{
			if (m_nFireCnt < m_nMaxFireCnt)
			{
				// 연사
				m_pAnimationController->SetTrackPosition(MONSTER_ONCE_TRACK_1, 0.3f);
				m_bCanFire = true;
				m_nFireCnt += 1;
			}
		}


		if (trackRate < 0.2f)
		{
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 0.5f);
		}
		else if (trackRate > 0.2f && trackRate < 0.3f)
		{
			if (m_bAttack2Lag)
			{
				m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 0.0f);
				m_ElapsedAttack2Delay += elapsedTime;
				if (m_ElapsedAttack2Delay >= m_Attack2Delay)
				{
					m_bAttack2Lag = false;
					m_ElapsedAttack2Delay = 0.0f;
				}
			}
			else
			{
				float animSpeed = 0.5f;
				if (m_bRage)
					animSpeed = 1.5f;

				if (m_nGunPattern == GunAttackPattern::Rapid_Shoot)
					m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, animSpeed * 2);
				else
					m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, animSpeed);
			}
		}
		else
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 0.7f);

		RotateToPlayer();

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.8f, 5.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			BlendWithIdleMovement(1);
			m_State = MonsterState::Monster_State_Idle;

			m_bCanFire = true;
			m_nFireCnt = 0;
			m_bAttackEndLag = true;

			// 다음 패턴 
			m_nPattern = rand() % 2;
		}
	}
		break;
	case Monster::Monster_State_Attack3:
		break;
	case Monster::Monster_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		if (trackRate < 0.5f)
		{
			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * -1;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);
		}

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.4f, 5.0f, MONSTER_ONCE_TRACK_1);

		// 피격 0.6f 이상시 종료 (피격 모션이 따로 없어)
		if (trackRate > 0.6f)
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
			BlendWithIdleMovement(1);
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
	m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 1.0f);

	if (m_HP / m_MaxHP < 0.5f)
		m_bRage = true;
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

	switch (m_nPattern)
	{
	case CyberTwins::Melee_Attack:
		if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 1.1f)
		{
			XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
			xmf3Accel.x = 0;
			xmf3Accel.z = 0;
			m_pBody->SetAcceleration(xmf3Accel);

			Attack1();
			return;
		}
		break;
	case CyberTwins::Gun_Attack:
		if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 10.0f)
		{
			XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
			xmf3Accel.x = 0;
			xmf3Accel.z = 0;
			m_pBody->SetAcceleration(xmf3Accel);

			Attack2();
			return;
		}
		break;
	default:
		break;
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

	UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
	m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
	m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

	m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, CyberTwins_Anim_Index_Attack1);

	m_State = MonsterState::Monster_State_Attack1;
}

void CyberTwins::Attack2()
{
	// 총 발사

	m_nGunPattern = rand() % 3;	// splash, chase, rapid
	m_nFireCnt = 0;
	m_bAttack2Lag = true;

	switch (m_nGunPattern)
	{
	case GunAttackPattern::Splash_Shoot:
		m_nMaxFireCnt = 0 + (m_bRage * 3);
		break;
	case GunAttackPattern::Chasing_Shoot:
		m_nMaxFireCnt = 2 + (m_bRage * 2);
		break;
	case GunAttackPattern::Rapid_Shoot:
		m_nMaxFireCnt = 4 + (m_bRage * 4);
		break;

	default:
		break;
	}

	UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
	m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
	m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

	m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, CyberTwins_Anim_Index_Attack2);

	m_State = MonsterState::Monster_State_Attack2;
}

void CyberTwins::GunFire()
{
	// 산탄
	// 유도탄
	// 연속 발사

	if (m_bCanFire)
	{
		m_bCanFire = false;

		XMVECTOR position = XMLoadFloat3(&m_xmf3Position);
		XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
		XMVECTOR up = XMLoadFloat3(&m_xmf3Up);
		XMVECTOR right = XMLoadFloat3(&m_xmf3Right);
		position += look * 18;
		position += up * 12;
		position += right * -8;
		XMFLOAT3 xmf3ProjectilePos;
		XMStoreFloat3(&xmf3ProjectilePos, position);

		if (m_nGunPattern == GunAttackPattern::Splash_Shoot)
		{
			for (int i = 0; i < 5; ++i)
			{
				std::shared_ptr<Object> tmp = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
					xmf3ProjectilePos, XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1),
					ENEMY_PROJECTILE_MODEL_NAME, 0);
				tmp->GetBody()->SetInGravity(false);

				XMVECTOR targetPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
				XMVECTOR velocity = targetPosition - position;
				velocity = XMVector3Normalize(velocity);
				XMVECTOR rotate = XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians((i * 15) - 30), 0);
				velocity = XMVector3Rotate(velocity, rotate);
				velocity *= 300;
				XMFLOAT3 xmf3Velocity;
				XMStoreFloat3(&xmf3Velocity, velocity);

				tmp->GetBody()->SetVelocity(xmf3Velocity);
			}
		}
		else
		{
			std::shared_ptr<Object> tmp = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
				xmf3ProjectilePos, XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1),
				ENEMY_PROJECTILE_MODEL_NAME, 0);
			tmp->GetBody()->SetInGravity(false);

			XMVECTOR targetPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
			XMVECTOR velocity = targetPosition - position;
			velocity = XMVector3Normalize(velocity);

			velocity *= 500;
			XMFLOAT3 xmf3Velocity;
			XMStoreFloat3(&xmf3Velocity, velocity);
			tmp->GetBody()->SetVelocity(xmf3Velocity);

			if (m_nGunPattern == GunAttackPattern::Chasing_Shoot)
			{
				Projectile* pProjectile = (Projectile*)tmp.get();
				pProjectile->SetChasePlayer(true);
				pProjectile->SetProjectileSpeed(150);
			}
		}
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

	InitAnimationTrack(2.0f);

	// 애니메이션 트랙 초기화
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, Necromancer_Anim_Index_Idle1);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Necromancer_Anim_Index_Walk);

	// 파괴 타이머 초기화
	m_DestroyTime = 5.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 20.0f;
	m_AttackRange = 27.0f;
	m_AttackRadius = 10.0f;

	// 체력 초기화
	m_MaxHP = 500.0f;
	m_HP = 500.0f;

	// 
	m_DefaultAccel = 500.0f;
	m_DefaultMaxSpeedXZ = 50.0f;

	return true;
}

void Necromancer::UpdateAnimationTrack(float elapsedTime)
{
	m_bSuperArmor = true;

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
		if (trackRate > 0.1f && trackRate < 0.8f)
		{
			m_pCollider->SetIsActive(false);

			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 7.0f);
			// 약간 앞으로 전진
			XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR deltaVelocity = l * 500;
			XMFLOAT3 xmf3DeltaVelocity;
			XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			m_pBody->AddVelocity(xmf3DeltaVelocity);

			CreateAttackSphere(0, m_xmf3ColliderExtents.x * 20, m_AttackDamage * 1.3f);
		}
		else if (trackRate > 0.8f)
		{
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);
			m_pCollider->SetIsActive(true);

			if (m_Attack1Cnt < m_bRage * 3)
				Attack1();
		}

		if (trackRate < 0.1f)
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
			m_Attack1Cnt = 0;

			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
			m_bAttackEndLag = true;

			// 다음 패턴 
			m_nPattern = rand() % 2;
			m_nAttackCnt++;
			if (m_nAttackCnt > m_nMaxAttackCnt)
				m_nPattern = NecromancerAttackPattern::Summon_Monster;
		}
	}
	break;
	case Monster::Monster_State_Attack2:
	{	
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.25f && trackRate < 0.3f)
			MagicMissile(m_bRage);
		else if (trackRate > 0.3f && trackRate < 0.35f)
			m_bCanFire = true;
		else if (trackRate > 0.35f && trackRate < 0.4f)
			MagicMissile(m_bRage);
		else if (trackRate > 0.4f && trackRate < 0.45f)
			m_bCanFire = true;
		else if (trackRate > 0.45f && trackRate < 0.5f)
			MagicMissile(m_bRage);
		else if (trackRate > 0.5f && trackRate < 0.55f)
			m_bCanFire = true;
		else if (trackRate > 0.77f)
			SplashMagic();
		
		RotateToPlayer();

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.3f, 10 / 3, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.8f, 5.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			BlendWithIdleMovement(1);
			m_State = MonsterState::Monster_State_Idle;

			m_bCanFire = true;
			m_bAttackEndLag = true;

			// 다음 패턴 
			m_nPattern = rand() % 2;
			m_nAttackCnt++;
			if (m_nAttackCnt > m_nMaxAttackCnt)
				m_nPattern = NecromancerAttackPattern::Summon_Monster;
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
		BlendAnimationToIdle(trackRate, 0.4f, 10.0f, MONSTER_ONCE_TRACK_1);

		// 두개의 피격모션이 섞여있어 절반만
		if (trackRate > 0.5f)
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			m_State = MonsterState::Monster_State_Idle;
			BlendWithIdleMovement(1);
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
	{
		m_nSummonCnt = 0;
		for (int i = 0; i < m_vpSummonedMonsters.size(); ++i)
		{
			if (!m_vpSummonedMonsters[i]->GetIsDestroying())
				m_nSummonCnt++;
		}
		if (m_nSummonCnt == 0 && m_bSummonDone)
			Special3();

		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		if(m_xmf3Position.y < m_FloatingHeight)
			m_pBody->SetVelocity(XMFLOAT3(0, 30, 0));
		else
		{
			m_pBody->SetVelocity(XMFLOAT3(0, 0, 0));
			m_pBody->SetInGravity(false);
		}

		if (trackRate > 0.5f)
		{
			if (m_bSummonDone)
			{
				if (m_bRage)
				{
					MagicMissile(true);
				}
			}
			else
			{
				SummonMonster();
				m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 1.0f);
				m_pBody->SetVelocity(XMFLOAT3(0, 0, 0));
			}
			m_pAnimationController->SetTrackPosition(MONSTER_ONCE_TRACK_1, 3.0f);
		}

		if (!m_bCanFire)
		{
			m_ElapsedFireTime += elapsedTime;
			if (m_ElapsedFireTime > m_FireRate)
			{
				m_bCanFire = true;
				m_ElapsedFireTime = 0.0f;
			}
		}
		RotateToPlayer();

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.8f, 5.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			BlendWithIdleMovement(1);
			m_State = MonsterState::Monster_State_Idle;

			m_bCanFire = true;
			m_bAttackEndLag = true;
			m_bSummonDone = false;

			// 다음 패턴 
			m_nPattern = rand() % 2;
			m_nAttackCnt++;
			if (m_nAttackCnt > m_nMaxAttackCnt)
				m_nPattern = NecromancerAttackPattern::Summon_Monster;
		}	
	}
		break;
	case Monster::Monster_State_Special3:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

		if (trackRate > 0.5f && m_bStunned)
		{
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 0.0f);

			m_ElapsedStunnedTime += elapsedTime;
			if (m_ElapsedStunnedTime > m_StunnedTime)
			{
				m_bStunned = false;
				m_ElapsedStunnedTime = 0.0f;
			}
		}
		else
			m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 2.0f);

		// 시작 블랜딩
		BlendIdleToAnimaiton(trackRate, 0.2f, 5.0f, MONSTER_ONCE_TRACK_1);
		// 종료 블랜딩
		BlendAnimationToIdle(trackRate, 0.8f, 5.0f, MONSTER_ONCE_TRACK_1);

		// 종료
		if (m_pAnimationController->GetTrackOver(MONSTER_ONCE_TRACK_1))
		{
			UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
			BlendWithIdleMovement(1);
			m_State = MonsterState::Monster_State_Idle;
			
			m_bCanFire = true;
			m_bAttackEndLag = true;
			
			// 다음 패턴 
			m_nPattern = rand() % 2;
			m_nAttackCnt++;
			if (m_nAttackCnt > m_nMaxAttackCnt)
				m_nPattern = NecromancerAttackPattern::Summon_Monster;
		}
	}
		break;
	default:
		break;
	}
}

void Necromancer::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	// 사운드 재생 테스트
	//mciSendString(_T("play Sound/Footstep01.wav"), 0, 0, 0);

	UINT hitAnimIdx = Necromancer_Anim_Index_Hit;
	UINT deathAnimIdx = Necromancer_Anim_Index_Death;

	Monster::ApplyDamage(power, xmf3DamageDirection, hitAnimIdx, deathAnimIdx);

	if (m_HP / m_MaxHP < 0.5f)
		m_bRage = true;
	if (m_State == MonsterState::Monster_State_Special3)
		m_pAnimationController->SetTrackPosition(MONSTER_ONCE_TRACK_1, 0.8f);
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

	switch (m_nPattern)
	{
	case NecromancerAttackPattern::Melee_Attack:
		if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 8.0f)
		{
			XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
			xmf3Accel.x = 0;
			xmf3Accel.z = 0;
			m_pBody->SetAcceleration(xmf3Accel);

			Attack1();
			return;
		}
		break;

	case NecromancerAttackPattern::Magic_Cast:
		if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 10.0f)
		{
			XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
			xmf3Accel.x = 0;
			xmf3Accel.z = 0;
			m_pBody->SetAcceleration(xmf3Accel);

			Attack2();
			return;
		}
		break;

	case NecromancerAttackPattern::Summon_Monster:
		Special2();
		break;

	default:
		break;
	}
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
		if (m_bAttackEndLag)
		{
			m_ElapsedAttackEndDelay += elapsedTime;
			if (m_ElapsedAttackEndDelay >= m_AttackEndDelay)
			{
				m_bAttackEndLag = false;
				m_ElapsedAttackEndDelay = 0.0f;
			}
			break;
		}
		Trace();
		break;
	default:
		break;
	}
}

void Necromancer::Attack1()
{
	m_Attack1Cnt += 1;

	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);

	UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
	m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
	m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);
	m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 0.75f);

	m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Necromancer_Anim_Index_Attack1);
	m_State = MonsterState::Monster_State_Attack1;
}

void Necromancer::Attack2()
{
	// 원거리 공격 - 매직 미사일
	UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
	m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
	m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

	m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Necromancer_Anim_Index_SpellCast);
	m_State = MonsterState::Monster_State_Attack2;
}

void Necromancer::Special2()
{
	m_pCollider->SetIsActive(false);

	// 몬스터 소환
	UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
	m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
	m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

	m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Necromancer_Anim_Index_Roar);
	m_State = MonsterState::Monster_State_Special2;
}

void Necromancer::Special3()
{
	BlendWithIdleMovement(1);
	m_bCanFire = true;
	m_bAttackEndLag = true;
	m_bSummonDone = false;
	m_bStunned = true;
	m_pBody->SetInGravity(true);
	m_pCollider->SetIsActive(true);

	// 그로기 상태
	UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
	m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
	m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);

	m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Necromancer_Anim_Index_Wound);
	m_State = MonsterState::Monster_State_Special3;
}

void Necromancer::MagicMissile(bool bChase)
{
	if (m_bCanFire)
	{
		m_bCanFire = false;

		XMVECTOR position = XMLoadFloat3(&m_xmf3Position);
		XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
		XMVECTOR up = XMLoadFloat3(&m_xmf3Up);
		XMVECTOR right = XMLoadFloat3(&m_xmf3Right);
		position += look * 18;
		position += up * 12;
		position += right * -6;
		XMFLOAT3 xmf3ProjectilePos;
		XMStoreFloat3(&xmf3ProjectilePos, position);


		std::shared_ptr<Object> tmp = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3ProjectilePos, XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1),
			ENEMY_PROJECTILE_MODEL_NAME, 0);
		tmp->GetBody()->SetInGravity(false);

		XMVECTOR targetPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
		XMVECTOR velocity = targetPosition - position;
		velocity = XMVector3Normalize(velocity);

		velocity *= 500;
		XMFLOAT3 xmf3Velocity;
		XMStoreFloat3(&xmf3Velocity, velocity);
		tmp->GetBody()->SetVelocity(xmf3Velocity);

		if (bChase)
		{
			Projectile* pProjectile = (Projectile*)tmp.get();
			pProjectile->SetChasePlayer(true);
			pProjectile->SetProjectileSpeed(200);
		}
	}
}

void Necromancer::SplashMagic()
{
	if (m_bCanFire)
	{
		m_bCanFire = false;

		XMVECTOR position = XMLoadFloat3(&m_xmf3Position);
		XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
		XMVECTOR up = XMLoadFloat3(&m_xmf3Up);
		XMVECTOR right = XMLoadFloat3(&m_xmf3Right);
		position += look * 18;
		position += up * 12;
		position += right * -6;
		XMFLOAT3 xmf3ProjectilePos;
		XMStoreFloat3(&xmf3ProjectilePos, position);

		for (int i = 0; i < 5; ++i)
		{
			std::shared_ptr<Object> tmp = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
				xmf3ProjectilePos, XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1),
				ENEMY_PROJECTILE_MODEL_NAME, 0);
			tmp->GetBody()->SetInGravity(false);

			XMVECTOR targetPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
			XMVECTOR velocity = targetPosition - position;
			velocity = XMVector3Normalize(velocity);
			XMVECTOR rotate = XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians((i * 15) - 30), 0);
			velocity = XMVector3Rotate(velocity, rotate);
			velocity *= 300;
			XMFLOAT3 xmf3Velocity;
			XMStoreFloat3(&xmf3Velocity, velocity);

			tmp->GetBody()->SetVelocity(xmf3Velocity);
		}
	}
}

void Necromancer::SummonMonster()
{
	if (m_nSummonCnt > 0)
		return;

	m_nAttackCnt = 0;
	if(m_bRage)
		m_nSummonPattern = rand() % 3 + 3;
	else
		m_nSummonPattern = rand() % 3;

	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	XMVECTOR centerPosition = myPosition;
	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	centerPosition += look * 10;
	XMVECTOR direction = centerPosition - myPosition;
	direction = XMVector3Normalize(direction);

	XMVECTOR summonPosition[5];
	XMFLOAT3 xmf3SummonPosition[5];
	for (int i = 0; i < 5; ++i)
	{
		XMVECTOR tmpDirection = direction;

		XMVECTOR rotate = XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians((i * 45) - 90), 0);
		tmpDirection = XMVector3Rotate(tmpDirection, rotate);
		summonPosition[i] = myPosition + tmpDirection * 60;

		XMStoreFloat3(&xmf3SummonPosition[i], summonPosition[i]);
	}

	////////////////////////////
	//
	//	0		N      4
	//      1      3
	//          2
	//
	////////////////////////////
	switch (m_nSummonPattern)
	{
	case SummonPattern::Summon_1:
	{
		// 일반 좀비 3마리
		std::shared_ptr<Object> pMonster1 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[1], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster2 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[2], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster3 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[3], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		
		m_vpSummonedMonsters.emplace_back(pMonster1);
		m_vpSummonedMonsters.emplace_back(pMonster2);
		m_vpSummonedMonsters.emplace_back(pMonster3);
	}
		break;
	case SummonPattern::Summon_2:
	{
		// 고급 좀비 1마리, 사마귀 2마리
		std::shared_ptr<Object> pMonster1 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[1], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			SCAVENGER_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster2 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[2], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			HIGHZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster3 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[3], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			SCAVENGER_MODEL_NAME, MONSTER_TRACK_CNT);

		m_vpSummonedMonsters.emplace_back(pMonster1);
		m_vpSummonedMonsters.emplace_back(pMonster2);
		m_vpSummonedMonsters.emplace_back(pMonster3);
	}
		break;
	case SummonPattern::Summon_3:
	{
		// 구울 2마리
		std::shared_ptr<Object> pMonster1 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[1], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			GHOUL_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster2 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[3], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			GHOUL_MODEL_NAME, MONSTER_TRACK_CNT);

		m_vpSummonedMonsters.emplace_back(pMonster1);
		m_vpSummonedMonsters.emplace_back(pMonster2);
	}
		break;
	case SummonPattern::Summon_4:
	{
		// 고급좀비 5마리
		std::shared_ptr<Object> pMonster1 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[0], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster2 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[1], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			SCAVENGER_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster3 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[2], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster4 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[3], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			SCAVENGER_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster5 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[4], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);

		m_vpSummonedMonsters.emplace_back(pMonster1);
		m_vpSummonedMonsters.emplace_back(pMonster2);
		m_vpSummonedMonsters.emplace_back(pMonster3);
	}
		break;
	case SummonPattern::Summon_5:
	{
		// 사마귀 5마리
		std::shared_ptr<Object> pMonster1 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[0], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster2 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[1], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			SCAVENGER_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster3 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[2], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster4 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[3], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			SCAVENGER_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster5 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[4], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);

		m_vpSummonedMonsters.emplace_back(pMonster1);
		m_vpSummonedMonsters.emplace_back(pMonster2);
		m_vpSummonedMonsters.emplace_back(pMonster3);
		m_vpSummonedMonsters.emplace_back(pMonster4);
		m_vpSummonedMonsters.emplace_back(pMonster5);
	}
		break;
	case SummonPattern::Summon_6:
	{
		// 구울 2마리, 사마리 3마리
		std::shared_ptr<Object> pMonster1 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[0], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster2 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[1], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			SCAVENGER_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster3 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[2], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster4 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[3], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			SCAVENGER_MODEL_NAME, MONSTER_TRACK_CNT);
		std::shared_ptr<Object> pMonster5 = Scene::CreateObject(g_pd3dDevice, g_pd3dCommandList,
			xmf3SummonPosition[4], XMFLOAT4(0, 0, 0, 1), m_xmf3Rotation, XMFLOAT3(1, 1, 1),
			ZOMBIE_MODEL_NAME, MONSTER_TRACK_CNT);

		m_vpSummonedMonsters.emplace_back(pMonster1);
		m_vpSummonedMonsters.emplace_back(pMonster2);
		m_vpSummonedMonsters.emplace_back(pMonster3);
		m_vpSummonedMonsters.emplace_back(pMonster4);
		m_vpSummonedMonsters.emplace_back(pMonster5);
	}
		break;
	default:
		break;
	}

	for (int i = 0; i < m_vpSummonedMonsters.size(); ++i)
	{
		Monster* pMonster = (Monster*)m_vpSummonedMonsters[i].get();
		pMonster->SetFindPlayer(true);
	}
	m_nSummonCnt = 0;
	for (int i = 0; i < m_vpSummonedMonsters.size(); ++i)
	{
		if (m_vpSummonedMonsters[i]->GetIsAlive())
			m_nSummonCnt++;
	}

	m_bSummonDone = true;
}
