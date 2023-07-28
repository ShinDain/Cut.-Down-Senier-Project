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
	void initializeState();

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
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection, UINT nHitAnimIdx, UINT nDeathAnimIdx);

public:
	virtual void StateAction(float elapsedTime);
	virtual void Patrol();
	virtual void Trace();

	virtual void Attack1() {}
	virtual void Attack2() {}
	virtual void Attack3() {}

	virtual void Special1() {}
	virtual void Special2() {}
	virtual void Special3() {}

protected:
	float m_SearchRadius = 50.0f;

	float m_AttackDamage = 0.0f;
	float m_AttackRange = 0.0f;
	float m_AttackRadius = 0.0f;

	XMFLOAT3 m_xmf3MonsterMovement = XMFLOAT3(0, 0, 0);

	bool m_bAttackEndLag = true;
	float m_AttackEndDelay = 0.5f;
	float m_ElapsedAttackEndDelay = 0.0f;

	bool m_bFindPlayer = false;
	bool m_bSuperArmor = false;

public:
	void SetFindPlayer(bool bFindPlayer) { m_bFindPlayer = bFindPlayer; }
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
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection);

public:
	virtual void Trace();

	virtual void Attack1();

};

class HighZombie : public Monster
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
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection);
public:
	virtual void Trace();

	virtual void Attack1();

};

class Scavenger : public Monster
{
public:
	Scavenger() {};
	Scavenger(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Scavenger(const Scavenger& rhs) = delete;
	Scavenger& operator=(const Scavenger& rhs) = delete;
	virtual ~Scavenger();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void UpdateAnimationTrack(float elapsedTime);

private:
	enum ScavengerAnimationIndex
	{
		Scavenger_Anim_Index_Idle1,
		Scavenger_Anim_Index_Idle2,
		Scavenger_Anim_Index_Idle3,
		Scavenger_Anim_Index_Walk,
		Scavenger_Anim_Index_Attack1,
		Scavenger_Anim_Index_Attack2,
		Scavenger_Anim_Index_Block,
		Scavenger_Anim_Index_Hit1,
		Scavenger_Anim_Index_Hit2,
		Scavenger_Anim_Index_Death1,
		Scavenger_Anim_Index_Death2,
		Scavenger_Anim_Index_Rage,
	};

public:
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection);
public:
	virtual void Trace();

	virtual void Attack1();

private:
	UINT m_nAttackCnt = 0;
	UINT m_nRushNum = 2;
	bool m_bRush = false;

	XMFLOAT3 m_xmf3RushTargetPosition = XMFLOAT3(0, 0, 0);

};

class Ghoul : public Monster
{
public:
	Ghoul() {};
	Ghoul(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Ghoul(const Ghoul& rhs) = delete;
	Ghoul& operator=(const Ghoul& rhs) = delete;
	virtual ~Ghoul();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Animate(float elapsedTime);
	virtual void UpdateAnimationTrack(float elapsedTime);

private:
	enum GhoulAnimationIndex
	{
		Ghoul_Anim_Index_Idle,
		Ghoul_Anim_Index_Walk,
		Ghoul_Anim_Index_Run,
		Ghoul_Anim_Index_Attack1,
		Ghoul_Anim_Index_Attack2,
		Ghoul_Anim_Index_Death		
	};

public:
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection);
public:
	virtual void Trace();

	virtual void Attack1();
};

class CyberTwins : public Monster
{
public:
	CyberTwins() {};
	CyberTwins(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	CyberTwins(const CyberTwins& rhs) = delete;
	CyberTwins& operator=(const CyberTwins& rhs) = delete;
	virtual ~CyberTwins();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void UpdateAnimationTrack(float elapsedTime);

private:
	enum CyberTwinsAnimationIndex
	{
		CyberTwins_Anim_Index_Idle,
		CyberTwins_Anim_Index_Run,
		CyberTwins_Anim_Index_Attack1,
		CyberTwins_Anim_Index_Attack2,
		CyberTwins_Anim_Index_Death
	};

	enum CyberTwinsAttackPattern
	{
		Melee_Attack,
		Gun_Attack
	};

	enum GunAttackPattern
	{
		Splash_Shoot,
		Chasing_Shoot,
		Rapid_Shoot
	};

public:
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection);
public:
	virtual void Trace();

	virtual void Attack1();
	virtual void Attack2();

	void GunFire();

private:
	UINT m_nPattern = CyberTwinsAttackPattern::Melee_Attack;

	UINT m_nGunPattern = GunAttackPattern::Rapid_Shoot;
	UINT m_nFireCnt = 0;
	UINT m_nMaxFireCnt = 0;
	bool m_bAttack2Lag = false;
	float m_Attack2Delay = 1.0f;
	float m_ElapsedAttack2Delay = 0.0f;

	bool m_bRage = false;
	bool m_bCanFire = true;

};

class Necromancer : public Monster
{
public:
	Necromancer() {};
	Necromancer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Necromancer(const Necromancer& rhs) = delete;
	Necromancer& operator=(const Necromancer& rhs) = delete;
	virtual ~Necromancer();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void UpdateAnimationTrack(float elapsedTime);

private:
	enum NecromancerAnimationIndex
	{
		Necromancer_Anim_Index_Idle1,
		Necromancer_Anim_Index_Idle2,
		Necromancer_Anim_Index_Walk,
		Necromancer_Anim_Index_Attack1,
		Necromancer_Anim_Index_Attack2,
		Necromancer_Anim_Index_Hit,
		Necromancer_Anim_Index_Death,
		Necromancer_Anim_Index_Roar,
		Necromancer_Anim_Index_SpellCast,
		Necromancer_Anim_Index_Wound
	};

	enum NecromancerAttackPattern
	{
		Melee_Attack,
		Magic_Cast,
		Summon_Monster
	};

	enum SummonPattern
	{
		Summon_1,		  // 일반 좀비 3마리, 사마귀 2마리
		Summon_2,		  // 고급 좀비 1마리, 일반 좀비 2마리
		Summon_3,		  // 구울 2마리
		Summon_4,		  // 사마귀 5마리
		Summon_5,		  // 고급좀비 3마리
		Summon_6		  // 구울 1마리, 사마리 3마리
	};

	struct SoundList
	{
		LPCWSTR IDLE1_SOUND = L"";
		LPCWSTR IDLE2_SOUND = L"";
		LPCWSTR IDLE3_SOUND = L"";
		LPCWSTR WALK1_SOUND = L"";
		LPCWSTR WALK2_SOUND = L"";
		LPCWSTR RUN1_SOUND = L"";
		LPCWSTR RUN2_SOUND = L"";
		LPCWSTR ATTACK1_SOUND = L"";
		LPCWSTR ATTACK2_SOUND = L"";
		LPCWSTR HIT1_SOUND = L"";
		LPCWSTR HIT2_SOUND = L"";
		LPCWSTR DEATH1_SOUND = L"";
		LPCWSTR DEATH2_SOUND = L"";
		LPCWSTR SPECIAL1_SOUND = L"";
		LPCWSTR SPECIAL2_SOUND = L"";
	};

public:
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection);
public:
	virtual void Trace();

	virtual void StateAction(float elapsedTime);

	virtual void Attack1();		// 낫 근접 공격
	virtual void Attack2();		// 원거리 공격

	virtual void Special2();	// 몬스터 소환
	virtual void Special3();	// 그로기

	void MagicMissile(bool bChase);
	void SplashMagic();

	void SummonMonster();

protected:
	UINT m_nSummonCnt = 0;
	bool m_bSummonDone = false;
	std::vector<std::shared_ptr<Object>> m_vpSummonedMonsters;

	float m_FloatingHeight = 50.0f;

	UINT m_nPattern = NecromancerAttackPattern::Melee_Attack;
	//UINT m_nPattern = NecromancerAttackPattern::Magic_Cast;
	UINT m_nSummonPattern = SummonPattern::Summon_1;
	UINT m_nMaxAttackCnt = 4;
	UINT m_nAttackCnt = 0;

	float m_Attack1Cnt = 0;

	bool m_bStunned = true;
	float m_StunnedTime = 1.5f;
	float m_ElapsedStunnedTime = 0.0f;

	bool m_bRage = false;

	bool m_bCanFire = true;
	float m_FireRate = 1.5f;
	float m_ElapsedFireTime = 0.0f;
};


#endif // !MONSTER_H
