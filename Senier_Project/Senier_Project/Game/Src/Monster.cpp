#include "../Header/Monster.h"

#define ZOMBIE_MAXSPEED 50.0f

#define MONSTER_IDLE_TRACK 0
#define MONSTER_MOVE_TRACK 1
#define MONSTER_LOOP_TRACK 2
#define MONSTER_ONCE_TRACK_1 3

// =========================================
// 기본 몬스터
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
		Cutting(XMFLOAT3(1, 0, 0));
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
	m_pAnimationController->SetTrackEnable(MONSTER_IDLE_TRACK, true);
	m_pAnimationController->SetTrackEnable(MONSTER_MOVE_TRACK, true);
	m_pAnimationController->SetTrackEnable(MONSTER_LOOP_TRACK, false);
	m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, false);
	m_pAnimationController->m_vpAnimationTracks[MONSTER_ONCE_TRACK_1]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, Zombie_Anim_Index_Idle);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Zombie_Anim_Index_Run);

	// 파괴 타이머 초기화
	m_DestroyTime = 3.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 10.0f;
	m_AttackRange = 6.0f;
	m_AttackRadius = 5.0f;

	// 애니메이션 기본 재생 속도
	m_AnimationSpeed = 1.5f;

	// 체력 초기화
	m_MaxHP = 100.0f;
	m_HP = 100.0f;

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

			m_pAnimationController->SetTrackEnable(MONSTER_IDLE_TRACK, false);
			m_pAnimationController->SetTrackEnable(MONSTER_MOVE_TRACK, false);
			m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 1);


			// 약간 앞으로 전진
			//XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
			//XMVECTOR deltaVelocity = l * 5;
			//XMFLOAT3 xmf3DeltaVelocity;
			//XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
			//m_pBody->AddVelocity(xmf3DeltaVelocity);
			CreateAttackSphere(m_AttackRange, m_AttackRadius, m_AttackDamage);
		}
		// 시작 블랜드
		else if (trackRate < 0.3f)
		{
			RotateToPlayer();

			float weight = trackRate * 10 / 3;
			BlendWithIdleMovement(1 - weight);
			m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, weight);
		}
		// 종료 블랜드
		else if (trackRate > 0.6f)
		{
			m_bSuperArmor = false;

			float weight = (trackRate - 0.6f) * 2.5f;

			m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 1 - weight);
			BlendWithIdleMovement(weight);
		}

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
	{	float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

	if (trackRate > 0.5f)
	{
		float weight = (trackRate - 0.5f) * 2;
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 1 - weight);

		BlendWithIdleMovement(weight);
	}
	else if (trackRate < 0.2f)
	{
		float weight = trackRate * 5;

		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, weight);

		BlendWithIdleMovement(1 - weight);
	}
	else
	{
		m_pAnimationController->SetTrackEnable(MONSTER_IDLE_TRACK, false);
		m_pAnimationController->SetTrackEnable(MONSTER_MOVE_TRACK, false);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 1);
	}

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

		if (trackRate < 0.2f)
		{
			float weight = trackRate * 5;

			m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, weight);

			BlendWithIdleMovement(1 - weight);
		}
		else
		{
			m_pAnimationController->SetTrackEnable(MONSTER_IDLE_TRACK, false);
			m_pAnimationController->SetTrackEnable(MONSTER_MOVE_TRACK, false);
			m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 1);
		}

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
	Monster::ApplyDamage(power, xmf3DamageDirection, Zombie_Anim_Index_Hit1, Zombie_Anim_Index_FallingBack);
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
	if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 0.8f * 3)
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
		m_State = MonsterState::Monster_State_Attack1;
		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Zombie_Anim_Index_Attack1);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);
		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, m_AnimationSpeed);

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

	// 애니메이션 트랙 초기화
	m_pAnimationController->SetTrackEnable(MONSTER_IDLE_TRACK, true);
	m_pAnimationController->SetTrackEnable(MONSTER_MOVE_TRACK, true);
	m_pAnimationController->SetTrackEnable(MONSTER_LOOP_TRACK, false);
	m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, false);
	m_pAnimationController->m_vpAnimationTracks[MONSTER_ONCE_TRACK_1]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, Zombie_Anim_Index_Idle);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Zombie_Anim_Index_Run);

	// 파괴 타이머 초기화
	m_DestroyTime = 3.0f;
	m_DissolveTime = 0.0f;

	// 공격력 및 반경 초기화
	m_AttackDamage = 10.0f;
	m_AttackRange = 6.0f;
	m_AttackRadius = 5.0f;

	// 애니메이션 기본 재생 속도
	m_AnimationSpeed = 2.0f;

	// 체력 초기화
	m_MaxHP = 100.0f;
	m_HP = 100.0f;

	return true;
}

void HighZombie::UpdateAnimationTrack(float elapsedTime)
{
	switch (m_State)
	{
	default:
		break;
	}
}

void HighZombie::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	Monster::ApplyDamage(power, xmf3DamageDirection, Zombie_Anim_Index_Hit1, Zombie_Anim_Index_FallingBack);
}

void HighZombie::StateAction(float elapsedTime)
{
	switch (m_State)
	{
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
	if (XMVectorGetX(XMVector3Length(accelDir)) < m_AttackRange * 0.8f * 3)
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
		m_State = MonsterState::Monster_State_Attack1;
		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Zombie_Anim_Index_Attack1);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);
		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 1.5f);

		break;
	}
}
