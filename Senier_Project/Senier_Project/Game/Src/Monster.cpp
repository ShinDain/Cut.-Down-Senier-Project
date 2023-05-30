#include "../Header/Monster.h"

Monster::Monster()
{
}

Monster::Monster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
				 ObjectInitData objData,
				 std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext)
{
	Character::Initialize(pd3dDevice, pd3dCommandList, objData, pModel, nAnimationTracks, pContext);

	m_pAnimationController->SetTrackEnable(0, true);
	m_pAnimationController->SetTrackEnable(1, true);
	m_pAnimationController->SetTrackEnable(2, false);
	m_pAnimationController->m_vpAnimationTracks[2]->SetType(ANIMATION_TYPE_ONCE);
	m_pAnimationController->SetTrackAnimationSet(0, Monster_Anim_Index_Idle);
	m_pAnimationController->SetTrackAnimationSet(1, Monster_Anim_Index_Run);
	m_pAnimationController->SetTrackAnimationSet(2, Monster_Anim_Index_Hit1);

}

Monster::~Monster()
{
	Destroy();
}

void Monster::Update(float elapsedTime)
{
	Character::Update(elapsedTime);

	Trace(elapsedTime);

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
		break;
	case Monster_State_Hit:
	{
		float trackRate = m_pAnimationController->GetTrackRate(2);
		if (trackRate > 0.5f)
		{
			float weight = (trackRate - 0.5f) * 2;
			m_pAnimationController->SetTrackWeight(2, 1 - weight);

			BlendWithIdleMovement(weight);
		}

		if (m_pAnimationController->GetTrackOver(2))
		{
			UnableAnimationTrack(2);
			m_AnimationState = MonsterAnimationState::Monster_State_Idle;
		}
	}
		break;
	case Monster_State_Death:
	{
		if (m_pAnimationController->GetTrackOver(2))
		{
			//UnableAnimationTrack(2);
			//m_AnimationState = MonsterAnimationState::Monster_State_Idle;

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

void Monster::Move(DWORD dwDirection)
{
}

void Monster::Trace(float elapsedTime)
{
	XMFLOAT3 xmf3TargetPosition = g_pPlayer->GetPosition();
	xmf3TargetPosition.y = 0;
	XMVECTOR targetPosition = XMLoadFloat3(&xmf3TargetPosition);

	XMFLOAT3 xmf3MyPosition = m_xmf3Position;
	xmf3MyPosition.y = 0;
	XMVECTOR myPosition = XMLoadFloat3(&xmf3MyPosition);
	
	XMVECTOR accelDir = targetPosition - myPosition;
	if (XMVectorGetX(XMVector3Length(accelDir)) < 25)
	{
		XMFLOAT3 xmf3Accel = m_pBody->GetAcceleration();
		xmf3Accel.x = 0;
		xmf3Accel.z = 0;
		m_pBody->SetAcceleration(xmf3Accel);
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
}

void Monster::OnHit()
{
}

void Monster::OnDeath()
{
}

void Monster::ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection)
{
	if (m_AnimationState == MonsterAnimationState::Monster_State_Death)
		return;

	Object::ApplyDamage(power, xmf3DamageDirection);

	m_pAnimationController->SetTrackEnable(0, false);
	m_pAnimationController->SetTrackEnable(1, false);

	if (m_HP > 0)
	{
		m_AnimationState = MonsterAnimationState::Monster_State_Hit;
		UnableAnimationTrack(2);
		m_pAnimationController->SetTrackEnable(2, true);
		m_pAnimationController->SetTrackAnimationSet(2, MonsterAnimationIndex::Monster_Anim_Index_Hit2);
		m_pAnimationController->SetTrackSpeed(2, 1.5f);
		m_pAnimationController->SetTrackWeight(2, 1);
	}
	else
	{
		m_AnimationState = MonsterAnimationState::Monster_State_Death;
		UnableAnimationTrack(2);
		m_pAnimationController->SetTrackEnable(2, true);
		m_pAnimationController->SetTrackAnimationSet(2, MonsterAnimationIndex::Monster_Anim_Index_FallingBack);
		m_pAnimationController->SetTrackSpeed(2, 1);
		m_pAnimationController->SetTrackWeight(2, 1);
	}
}
