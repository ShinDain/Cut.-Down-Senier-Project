#include "../Header/Monster.h"

#define ZOMBIE_MAXSPEED 50.0f

#define ZOMBIE_IDLE_TRACK 0
#define ZOMBIE_MOVE_TRACK 1
#define ZOMBIE_LOOP_TRACK 2
#define ZOMBIE_ONCE_TRACK_1 3

Monster::Monster()
{
}

Monster::Monster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
				 ObjectInitData objData,
				 std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	m_pAnimationController->SetTrackEnable(ZOMBIE_IDLE_TRACK, true);
	m_pAnimationController->SetTrackEnable(ZOMBIE_MOVE_TRACK, true);
	m_pAnimationController->SetTrackEnable(ZOMBIE_LOOP_TRACK, false);
	m_pAnimationController->SetTrackEnable(ZOMBIE_ONCE_TRACK_1, false);
	m_pAnimationController->m_vpAnimationTracks[ZOMBIE_ONCE_TRACK_1]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->SetTrackAnimationSet(ZOMBIE_IDLE_TRACK, Monster_Anim_Index_Idle);
	m_pAnimationController->SetTrackAnimationSet(ZOMBIE_MOVE_TRACK, Monster_Anim_Index_Run);

	m_DestroyTime = 1.0f;
}

Monster::~Monster()
{
	Destroy();
}

void Monster::Update(float elapsedTime)
{
	Character::Update(elapsedTime);

	XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
	xmf3Accel.x = 0;
	xmf3Accel.z = 0;
	m_pBody->SetAcceleration(xmf3Accel);

	// state에 따른 행동
	StateAction(elapsedTime);

	{
		XMFLOAT3 tmp =	m_pBody->GetVelocity();
		XMFLOAT3 tmp2 =	m_pBody->GetAcceleration();
		float a = 0;
	}
}

void Monster::Destroy()
{
	Object::Destroy();
}

void Monster::UpdateAnimationTrack(float elapsedTime)
{
	switch (m_AnimationState)
	{
	case Monster_State_Idle:
	case Monster_State_Trace:
	{
		BlendWithIdleMovement(1);
	}
		break;
	case Monster_State_Jump:
		break;
	case Monster_State_Falling:
		break;
	case Monster_State_Land:
		break;
	case Monster_State_Melee:
	{
		float trackRate = m_pAnimationController->GetTrackRate(ZOMBIE_ONCE_TRACK_1);
		// 공격 판정
		if (trackRate > 0.3f && trackRate < 0.4f)
		{
			m_bSuperArmor = true;

			m_pAnimationController->SetTrackEnable(ZOMBIE_IDLE_TRACK, false);
			m_pAnimationController->SetTrackEnable(ZOMBIE_MOVE_TRACK, false);
			m_pAnimationController->SetTrackWeight(ZOMBIE_ONCE_TRACK_1, 1);

			CreateAttackSphere();
		}
		// 시작 블랜드
		else if (trackRate < 0.3f)
		{
			RotateToPlayer();

			float weight = trackRate * 10 / 3;
			BlendWithIdleMovement(1 - weight);
			m_pAnimationController->SetTrackWeight(ZOMBIE_ONCE_TRACK_1, weight);
		}
		// 종료 블랜드
		else if (trackRate > 0.6f)
		{
			m_bSuperArmor = false;

			float weight = (trackRate - 0.6f) * 2.5f;

			m_pAnimationController->SetTrackWeight(ZOMBIE_ONCE_TRACK_1, 1 - weight);
			BlendWithIdleMovement(weight);
		}

		// 종료
		if (m_pAnimationController->GetTrackOver(ZOMBIE_ONCE_TRACK_1))
		{
			UnableAnimationTrack(ZOMBIE_ONCE_TRACK_1);
			m_AnimationState = MonsterAnimationState::Monster_State_Idle;
		}
	}
		break;
	case Monster_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(ZOMBIE_ONCE_TRACK_1);

		if (trackRate > 0.5f)
		{
			float weight = (trackRate - 0.5f) * 2;
			m_pAnimationController->SetTrackWeight(ZOMBIE_ONCE_TRACK_1, 1 - weight);

			BlendWithIdleMovement(weight);
		}
		else if (trackRate < 0.2f)
		{
			float weight = trackRate * 5;

			m_pAnimationController->SetTrackWeight(ZOMBIE_ONCE_TRACK_1, weight);

			BlendWithIdleMovement(1 - weight);
		}
		else
		{
			m_pAnimationController->SetTrackEnable(ZOMBIE_IDLE_TRACK, false);
			m_pAnimationController->SetTrackEnable(ZOMBIE_MOVE_TRACK, false);
			m_pAnimationController->SetTrackWeight(ZOMBIE_ONCE_TRACK_1, 1);
		}

		if (m_pAnimationController->GetTrackOver(ZOMBIE_ONCE_TRACK_1))
		{
			UnableAnimationTrack(ZOMBIE_ONCE_TRACK_1);
			m_AnimationState = MonsterAnimationState::Monster_State_Idle;
		}
	}
		break;
	case Monster_State_Death:
	{
		float trackRate = m_pAnimationController->GetTrackRate(ZOMBIE_ONCE_TRACK_1);

		if (trackRate < 0.2f)
		{
			float weight = trackRate * 5;

			m_pAnimationController->SetTrackWeight(ZOMBIE_ONCE_TRACK_1, weight);

			BlendWithIdleMovement(1 - weight);
		}
		else
		{
			m_pAnimationController->SetTrackEnable(ZOMBIE_IDLE_TRACK, false);
			m_pAnimationController->SetTrackEnable(ZOMBIE_MOVE_TRACK, false);
			m_pAnimationController->SetTrackWeight(ZOMBIE_ONCE_TRACK_1, 1);
		}

		if (m_pAnimationController->GetTrackOver(ZOMBIE_ONCE_TRACK_1))
		{
			m_bDestroying = true;
		}
	}
		break;
	default:
		break;
	}

}

void Monster::StateAction(float elapsedTime)
{
	switch (m_AnimationState)
	{
	case Monster_State_Idle:
		if (m_bFindPlayer) m_AnimationState = Monster_State_Trace;
		Patrol();
		break;
	case Monster_State_Trace:
		Trace();
		break;
	default:
		break;
	}
}

void Monster::Move(DWORD dwDirection)
{
}

void Monster::DoLanding()
{
	m_bIsFalling = false;
	m_MaxSpeedXZ = ZOMBIE_MAXSPEED;
	m_CharacterFriction = 350.0f;
	m_Acceleration = 500.0f;
}

void Monster::Patrol()
{
	// 패트롤, 왔다 갔다
	BoundingSphere boundSphere;
	boundSphere.Center = m_xmf3Position;
	boundSphere.Radius = 20.0f;
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

void Monster::Attack()
{
	XMVECTOR playerPosition = XMLoadFloat3(&g_pPlayer->GetPosition());
	XMVECTOR myPosition = XMLoadFloat3(&m_xmf3Position);
	if (XMVectorGetX(XMVector3Length(myPosition - playerPosition)) > m_AttackRange * 3)
	{
		m_AnimationState = Monster_State_Trace;
		return;
	}

	switch (m_AnimationState)
	{
	/*case Monster_State_Idle:
		break;
	case Monster_State_Jump:
		break;
	case Monster_State_Falling:
		break;
	case Monster_State_Land:
		break;*/
	case Monster_State_Melee:
		break;
	default:
		m_AnimationState = MonsterAnimationState::Monster_State_Melee;
		UnableAnimationTrack(ZOMBIE_ONCE_TRACK_1);
		m_pAnimationController->SetTrackEnable(ZOMBIE_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(ZOMBIE_ONCE_TRACK_1, Monster_Anim_Index_Attack1);
		m_pAnimationController->SetTrackWeight(ZOMBIE_ONCE_TRACK_1, 0);
		m_pAnimationController->SetTrackSpeed(ZOMBIE_ONCE_TRACK_1, 1.5f);

		break;
	}

}

void Monster::CreateAttackSphere()
{
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
		g_pPlayer->ApplyDamage(10, m_xmf3Look);
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

void Monster::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	if (m_AnimationState == MonsterAnimationState::Monster_State_Death || m_bInvincible)
		return;

	Character::ApplyDamage(power, xmf3DamageDirection);

	if (m_bSuperArmor)
		return;

	UnableAnimationTrack(ZOMBIE_ONCE_TRACK_1);
	if (m_HP > 0)
	{
		m_AnimationState = MonsterAnimationState::Monster_State_Hit;
		m_pAnimationController->SetTrackEnable(ZOMBIE_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(ZOMBIE_ONCE_TRACK_1, MonsterAnimationIndex::Monster_Anim_Index_Hit2);
		m_pAnimationController->SetTrackSpeed(ZOMBIE_ONCE_TRACK_1, 1.5f);
	}
	else
	{
		Cutting(XMFLOAT3(1, 0, 0));
		m_AnimationState = MonsterAnimationState::Monster_State_Death;
		m_pAnimationController->SetTrackEnable(ZOMBIE_ONCE_TRACK_1, true);
		m_pAnimationController->SetTrackAnimationSet(ZOMBIE_ONCE_TRACK_1, MonsterAnimationIndex::Monster_Anim_Index_FallingBack);
		m_pAnimationController->SetTrackSpeed(ZOMBIE_ONCE_TRACK_1, 1);
	}
}
