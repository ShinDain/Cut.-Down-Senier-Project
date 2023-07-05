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
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext) {}

	Monster(const Monster& rhs) = delete;
	Monster& operator=(const Monster& rhs) = delete;
	virtual ~Monster() { Destroy(); }

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext) {
		return false;
	}

	virtual void InitAnimationTrack(float animationSpeed);

	virtual void Update(float elapsedTime);
	virtual void UpdateAnimationTrack(float elapsedTime);
	virtual void CreateAttackSphere(float range, float radius, float damage);

protected:
	enum MonsterState
	{
		Monster_State_Idle,
		Monster_State_Trace,
		Monster_State_Attack1,
		Monster_State_Attack2,
		Monster_State_Attack3,
		Monster_State_Hit,
		Monster_State_Death,
		Monster_State_Special1,
		Monster_State_Special2,
		Monster_State_Special3
	};

	MonsterState m_State = MonsterState::Monster_State_Idle;

public:
	virtual void MonsterMove(XMFLOAT3 xmf3Direction);
	virtual void RotateToPlayer();
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, UINT nHitAnimIdx, UINT nDeathAnimIdx);

public:
	virtual void StateAction(float elapsedTime) {}
	virtual void Patrol();
	virtual void Trace();

	virtual void Attack1() {}
	virtual void Attack2() {}
	virtual void Attack3() {}

	virtual void Special1() {}
	virtual void Special2() {}
	virtual void Special3() {}

protected:
	float m_SearchRadius = 30.0f;

	float m_AttackDamage = 0.0f;
	float m_AttackRange = 0.0f;
	float m_AttackRadius = 0.0f;

	XMFLOAT3 m_xmf3MonsterMovement = XMFLOAT3(0, 0, 0);

	bool m_bFindPlayer = false;
	bool m_bSuperArmor = false;
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

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void UpdateAnimationTrack(float elapsedTime);

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

public:
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection);

public:
	virtual void StateAction(float elapsedTime);
	virtual void Trace();

	virtual void Attack1();

protected:

};

class HighZombie : public Zombie
{
public:
	HighZombie() {};
	HighZombie(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	HighZombie(const HighZombie& rhs) = delete;
	HighZombie& operator=(const HighZombie& rhs) = delete;
	virtual ~HighZombie();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void UpdateAnimationTrack(float elapsedTime);

private:
	enum HighZombieAnimationIndex
	{
		HighZombie_Anim_Index_Idle1,
		HighZombie_Anim_Index_Idle2,
		HighZombie_Anim_Index_Roar,
		HighZombie_Anim_Index_Walk,
		HighZombie_Anim_Index_Run,
		HighZombie_Anim_Index_Attack1,
		HighZombie_Anim_Index_Attack2,
		HighZombie_Anim_Index_Attack3,
		HighZombie_Anim_Index_Attack4,
		HighZombie_Anim_Index_Hit,
		HighZombie_Anim_Index_Death1,
		HighZombie_Anim_Index_Death2
	};

public:
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection);
public:
	virtual void StateAction(float elapsedTime);
	virtual void Trace();

	virtual void Attack1();
protected:

};

#endif // !MONSTER_H
