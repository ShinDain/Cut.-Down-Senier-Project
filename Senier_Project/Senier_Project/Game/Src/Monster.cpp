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
}

void Monster::Destroy()
{
	Character::Destroy();
}

void Monster::UpdateAnimationTrack()
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
		break;
	default:
		break;
	}

}



void Monster::Move(DWORD dwDirection)
{
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
	Object::ApplyDamage(power, xmf3DamageDirection);

	m_AnimationState = MonsterAnimationState::Monster_State_Hit;
	m_pAnimationController->SetTrackAnimationSet(2, MonsterAnimationIndex::Monster_Anim_Index_Hit2);
	m_pAnimationController->SetTrackEnable(2, true);
	m_pAnimationController->SetTrackPosition(2, 0.1f);
	m_pAnimationController->SetTrackSpeed(2, 1.5f);
	m_pAnimationController->SetTrackWeight(2, 1);

	m_pAnimationController->SetTrackEnable(0, false);
	m_pAnimationController->SetTrackEnable(1, false);
}
