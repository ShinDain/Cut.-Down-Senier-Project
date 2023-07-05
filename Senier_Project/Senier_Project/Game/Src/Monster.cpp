#include "../Header/Monster.h"

#define MONSTER_ACCELERATION 500.f;

#define ZOMBIE_MAXSPEED 50.0f

#define MONSTER_IDLE_TRACK 0
#define MONSTER_MOVE_TRACK 1
#define MONSTER_LOOP_TRACK 2
#define MONSTER_ONCE_TRACK_1 3

// =========================================
// 애니메이션 테스트용 기본 몬스터
Monster::Monster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	m_pAnimationController->SetTrackEnable(MONSTER_IDLE_TRACK, true);
	m_pAnimationController->SetTrackEnable(MONSTER_MOVE_TRACK, true);
	m_pAnimationController->SetTrackEnable(2, false);
	m_pAnimationController->SetTrackEnable(3, false);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, m_tmpAnimationIdx);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, m_tmpAnimationIdx);

	m_DestroyTime = 3.0f;
	m_DissolveTime = 0.0f;
}

Monster::~Monster()
{
	Destroy();
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

void Monster::KeyDownEvent(WPARAM wParam)
{
	if (wParam == 'L')
	{
		m_tmpAnimationIdx += 1;
		m_tmpAnimationIdx = m_tmpAnimationIdx % m_pAnimationController->m_pAnimationSets->m_nAnimationSets;

		m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, m_tmpAnimationIdx);
	}

}

void Monster::MonsterMove(XMFLOAT3 xmf3Direction)
{
	XMVECTOR monsterMovement = XMLoadFloat3(&xmf3Direction);
	monsterMovement *= MONSTER_ACCELERATION;

	XMStoreFloat3(&m_xmf3MonsterMovement, monsterMovement);
}

void Monster::DoLanding()
{
	m_bIsFalling = false;
	m_MaxSpeedXZ = ZOMBIE_MAXSPEED;
	m_CharacterFriction = 350.0f;
	m_Acceleration = 500.0f;
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

/////////////////////////////
// 일반 좀비
/////////////////////////////
Zombie::Zombie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	m_pAnimationController->SetTrackEnable(MONSTER_IDLE_TRACK, true);
	m_pAnimationController->SetTrackEnable(MONSTER_MOVE_TRACK, true);
	m_pAnimationController->SetTrackEnable(MONSTER_LOOP_TRACK, false);
	m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, false);
	m_pAnimationController->m_vpAnimationTracks[MONSTER_ONCE_TRACK_1]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_IDLE_TRACK, Zombie_Anim_Index_Idle);
	m_pAnimationController->SetTrackAnimationSet(MONSTER_MOVE_TRACK, Zombie_Anim_Index_Run);

	m_DestroyTime = 3.0f;
	m_DissolveTime = 0.0f;
}

Zombie::~Zombie()
{
	Destroy();
}

void Zombie::UpdateAnimationTrack(float elapsedTime)
{
	switch (m_AnimationState)
	{
	case Zombie_State_Idle:
	case Zombie_State_Trace:
	{
		BlendWithIdleMovement(1);
	}
	break;
	case Zombie_State_Jump:
		break;
	case Zombie_State_Falling:
		break;
	case Zombie_State_Land:
		break;
	case Zombie_State_Melee:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.3f && trackRate < 0.4f)
		{
			m_bSuperArmor = true;

			m_pAnimationController->SetTrackEnable(MONSTER_IDLE_TRACK, false);
			m_pAnimationController->SetTrackEnable(MONSTER_MOVE_TRACK, false);
			m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 1);

			CreateAttackSphere();
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
			m_AnimationState = ZombieAnimationState::Zombie_State_Idle;
		}
	}
	break;
	case Zombie_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(MONSTER_ONCE_TRACK_1);

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
			m_AnimationState = ZombieAnimationState::Zombie_State_Idle;
		}
	}
	break;
	case Zombie_State_Death:
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
	default:
		break;
	}

}

void Zombie::StateAction(float elapsedTime)
{
	switch (m_AnimationState)
	{
	case Zombie_State_Idle:
		Patrol();
		if (m_bFindPlayer) m_AnimationState = Zombie_State_Trace;
		break;
	case Zombie_State_Trace:
		Trace();
		break;
	default:
		break;
	}
}

void Zombie::Patrol()
{
	if (!g_pPlayer->GetIsAlive())
		return;

	// 패트롤, 왔다 갔다



	// 탐지 위치 시점 앞으로 이동
	// 플레이어 탐지 시 추적 상태로 변경
	XMVECTOR searchPos = XMLoadFloat3(&m_xmf3Position);
	XMVECTOR look = XMLoadFloat3(&m_xmf3Look);
	searchPos = searchPos + 7 * look;
	XMFLOAT3 xmf3SearchPos;
	XMStoreFloat3(&xmf3SearchPos, searchPos);

	BoundingSphere boundSphere;
	boundSphere.Center = xmf3SearchPos;
	boundSphere.Radius = 20.0f;
	BoundingOrientedBox playerCollider = std::static_pointer_cast<ColliderBox>(g_pPlayer->GetCollider())->GetOBB();
	if (boundSphere.Intersects(playerCollider))
		m_bFindPlayer = true;
}

void Zombie::Trace()
{
	// 맹목적으로 플레이어를 추적한다.
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

		Attack();
		return;
	}

	accelDir = XMVector3Normalize(accelDir);
	XMVECTOR deltaAccelXZ = accelDir * m_Acceleration;
	XMFLOAT3 xmf3DeltaAccelXZ;
	XMStoreFloat3(&xmf3DeltaAccelXZ, deltaAccelXZ);

	XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
	xmf3Accel.x = xmf3DeltaAccelXZ.x;
	xmf3Accel.z = xmf3DeltaAccelXZ.z;
	m_pBody->SetAcceleration(xmf3Accel);
}

void Zombie::Attack()
{
	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	if (XMVectorGetX(XMVector3Length(myPosition - playerPosition)) > m_AttackRange * 3)
	{
		m_AnimationState = Zombie_State_Trace;
		return;
	}

	switch (m_AnimationState)
	{
		/*case Zombie_State_Idle:
			break;
		case Zombie_State_Jump:
			break;
		case Zombie_State_Falling:
			break;
		case Zombie_State_Land:
			break;*/
	case Zombie_State_Melee:
		break;
	default:
		m_AnimationState = ZombieAnimationState::Zombie_State_Melee;
		UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, Zombie_Anim_Index_Attack1);
		m_pAnimationController->SetTrackWeight(MONSTER_ONCE_TRACK_1, 0);
		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 1.5f);

		break;
	}

}

void Zombie::CreateAttackSphere()
{
	if (!g_pPlayer->GetIsAlive())
		return;

	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	XMVECTOR attackPosition = myPosition + (l * m_AttackRange);

	// 약간 앞으로 전진
	XMVECTOR deltaVelocity = l * 5;
	XMFLOAT3 xmf3DeltaVelocity;
	XMStoreFloat3(&xmf3DeltaVelocity, deltaVelocity);
	m_pBody->AddVelocity(xmf3DeltaVelocity);

	BoundingSphere attackSphere;
	XMStoreFloat3(&attackSphere.Center, attackPosition);
	attackSphere.Radius = m_AttackRadius;
	BoundingOrientedBox playerCollider = std::static_pointer_cast<ColliderBox>(g_pPlayer->GetCollider())->GetOBB();
	if (attackSphere.Intersects(playerCollider))
		g_pPlayer->ApplyDamage(m_AttackDamage, m_xmf3Look);
}

void Zombie::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	if (m_AnimationState == ZombieAnimationState::Zombie_State_Death || m_bInvincible)
		return;

	Character::ApplyDamage(power, xmf3DamageDirection);

	if (m_bSuperArmor)
		return;

	UnableAnimationTrack(MONSTER_ONCE_TRACK_1);
	if (m_HP > 0)
	{
		m_AnimationState = ZombieAnimationState::Zombie_State_Hit;
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, ZombieAnimationIndex::Zombie_Anim_Index_Hit2);
		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 1.5f);
	}
	else
	{
		Cutting(XMFLOAT3(1, 0, 0));
		m_AnimationState = ZombieAnimationState::Zombie_State_Death;
		m_pAnimationController->SetTrackEnable(MONSTER_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(MONSTER_ONCE_TRACK_1, ZombieAnimationIndex::Zombie_Anim_Index_FallingBack);
		m_pAnimationController->SetTrackSpeed(MONSTER_ONCE_TRACK_1, 1);
	}
}
/////////////////////////////
// 고급 좀비
/////////////////////////////
//HighZombie::HighZombie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ObjectInitData objData, std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
//{
//}
//
//HighZombie::~HighZombie()
//{
//}
//
//void HighZombie::UpdateAnimationTrack(float elapsedTime)
//{
//}
//
//void HighZombie::StateAction(float elapsedTime)
//{
//}
//
//void HighZombie::Patrol()
//{
//}
//
//void HighZombie::Trace()
//{
//}
//
//void HighZombie::Attack()
//{
//}
//
//void HighZombie::CreateAttackSphere()
//{
//}
//
//void HighZombie::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
//{
//}
//