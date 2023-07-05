#ifndef MONSTER_H

#define MONSTER_H

#include "../../Common/Header/D3DUtil.h"
#include "Character.h"

class Monster : public Character
{
public:
	Monster() {};
	Monster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Monster(const Monster& rhs) = delete;
	Monster& operator=(const Monster& rhs) = delete;
	virtual ~Monster();

	virtual void Update(float elapsedTime);
	virtual void UpdateAnimationTrack(float elapsedTime);
	virtual void KeyDownEvent(WPARAM wParam);

public:
	virtual void MonsterMove(XMFLOAT3 xmf3Direction);
	virtual void DoLanding();
	virtual void RotateToPlayer();

public:
	virtual void StateAction(float elapsedTime) {}
	virtual void Patrol() {}
	virtual void Trace() {}

	virtual void Attack() {}
	virtual void CreateAttackSphere() {}

	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection) {}

protected:
	UINT m_tmpAnimationIdx = 0;

	XMFLOAT3 m_xmf3MonsterMovement = XMFLOAT3(0, 0, 0);
};

class Zombie : public Monster
{
public:
	Zombie() {};
	Zombie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Zombie(const Zombie& rhs) = delete;
	Zombie& operator=(const Zombie& rhs) = delete;
	virtual ~Zombie();

	virtual void UpdateAnimationTrack(float elapsedTime);
	virtual void KeyDownEvent(WPARAM wParam) {}

private:
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
	ZombieAnimationState m_AnimationState = ZombieAnimationState::Zombie_State_Idle;

public:
	virtual void StateAction(float elapsedTime);
	virtual void Patrol();
	virtual void Trace();

	virtual void Attack();
	virtual void CreateAttackSphere();

	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection);

protected:
	float m_AttackDamage = 10.0f;
	float m_AttackRange = 6.0f;
	float m_AttackRadius = 5.0f;

	bool m_bFindPlayer = false;
	bool m_bSuperArmor = false;


};

//class HighZombie : public Zombie
//{
//public:
//	HighZombie() {};
//	HighZombie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
//		ObjectInitData objData,
//		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);
//
//	HighZombie(const HighZombie& rhs) = delete;
//	HighZombie& operator=(const HighZombie& rhs) = delete;
//	virtual ~HighZombie();
//
//	virtual void UpdateAnimationTrack(float elapsedTime);
//	virtual void KeyDownEvent(WPARAM wParam) {}
//
//private:
//	enum HighZombieAnimationIndex
//	{
//		Zombie_Anim_Index_Idle,
//		Zombie_Anim_Index_Walk1,
//		Zombie_Anim_Index_Walk2,
//		Zombie_Anim_Index_Run,
//		Zombie_Anim_Index_Attack1,
//		Zombie_Anim_Index_Attack2,
//		Zombie_Anim_Index_FallingBack,
//		Zombie_Anim_Index_FallingForward,
//		Zombie_Anim_Index_Hit1,
//		Zombie_Anim_Index_Hit2
//	};
//
//	enum HighZombieAnimationState
//	{
//		Zombie_State_Idle,
//		Zombie_State_Trace,
//		Zombie_State_Jump,
//		Zombie_State_Falling,
//		Zombie_State_Land,
//		Zombie_State_Melee,
//		Zombie_State_Hit,
//		Zombie_State_Death
//	};
//	HighZombieAnimationState m_AnimationState = HighZombieAnimationState::Zombie_State_Idle;
//
//
//public:
//	virtual void StateAction(float elapsedTime);
//	virtual void Patrol();
//	virtual void Trace();
//
//	virtual void Attack();
//	virtual void CreateAttackSphere();
//
//	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection);
//
//protected:
//
//};
//
#endif // !MONSTER_H
