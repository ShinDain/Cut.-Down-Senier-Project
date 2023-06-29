#ifndef MONSTER_H

#define MONSTER_H

#include "../../Common/Header/D3DUtil.h"
#include "Character.h"

enum BossAnimationIndex
{
	Boss_Anim_Index_Idle,
	Boss_Anim_Index_Run,
	Boss_Anim_Index_Attack1,
	Boss_Anim_Index_Attack2,
	Boss_Anim_Index_Death
};

enum ZombieAnimationIndex
{
	Zombie_Anim_Index_Idle,
	Zombie_Anim_Index_Walk1,
	Zombie_Anim_Index_Walk2,
	Zombie_Anim_Index_Run,
	Zombie_Anim_Index_Attack1,
	Zombie_Anim_Index_Attack2,
	Zombie_Anim_Index_FallingBack,
	Zombie_Anim_Index_FallingForward,
	Zombie_Anim_Index_Hit1,
	Zombie_Anim_Index_Hit2
};

enum ZombieAnimationState
{
	Zombie_State_Idle,
	Zombie_State_Trace,
	Zombie_State_Jump,
	Zombie_State_Falling,
	Zombie_State_Land,
	Zombie_State_Melee,
	Zombie_State_Hit,
	Zombie_State_Death
};

class Monster : public Character
{
public:
	Monster();
	Monster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Monster(const Monster& rhs) = delete;
	Monster& operator=(const Monster& rhs) = delete;
	virtual ~Monster();

	virtual void Update(float elapsedTime) {}
	virtual void Destroy() {}
	virtual void UpdateAnimationTrack(float elapsedTime) {}

public:
	virtual void StateAction(float elapsedTime) {}

	virtual void Move(DWORD dwDirection) {}
	virtual void DoLanding() {}
	virtual void Patrol() {}
	virtual void Trace() {}

	virtual void Attack() {}
	virtual void CreateAttackSphere() {}
	virtual void RotateToPlayer() {}

	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection) {}

protected:
	float m_AttackRange = 6.0f;
	float m_AttackRadius = 5.0f;

	bool m_bFindPlayer = false;
	bool m_bSuperArmor = false;
};

class Zombie : public Monster
{
public:
	Zombie();
	Zombie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Zombie(const Zombie& rhs) = delete;
	Zombie& operator=(const Zombie& rhs) = delete;
	virtual ~Zombie();

	virtual void Update(float elapsedTime);
	virtual void Destroy();
	virtual void UpdateAnimationTrack(float elapsedTime);

public:
	virtual void StateAction(float elapsedTime);

	virtual void Move(DWORD dwDirection);
	virtual void DoLanding();
	virtual void Patrol();
	virtual void Trace();

	virtual void Attack();
	virtual void CreateAttackSphere();
	virtual void RotateToPlayer();

	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection);

protected:
	ZombieAnimationState m_AnimationState = ZombieAnimationState::Zombie_State_Idle;

};


#endif // !MONSTER_H
