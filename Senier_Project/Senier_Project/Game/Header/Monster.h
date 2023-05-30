#ifndef MONSTER_H

#define MONSTER_H

#include "../../Common/Header/D3DUtil.h"
#include "Character.h"

enum MonsterAnimationIndex
{
	Monster_Anim_Index_Idle,
	Monster_Anim_Index_Walk1,
	Monster_Anim_Index_Walk2,
	Monster_Anim_Index_Run,
	Monster_Anim_Index_Attack,
	Monster_Anim_Index_FallingBack,
	Monster_Anim_Index_FallingForward,
	Monster_Anim_Index_Hit1,
	Monster_Anim_Index_Hit2
};

enum MonsterAnimationState
{
	Monster_State_Idle,
	Monster_State_Jump,
	Monster_State_Falling,
	Monster_State_Land,
	Monster_State_Melee,
	Monster_State_Hit,
	Monster_State_Death
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

	virtual void Update(float elapsedTime);

	virtual void Destroy();

	virtual void UpdateAnimationTrack(float elapsedTime);

public:
	virtual void ProcessInput(UCHAR* pKeybuffer) {}
	virtual void KeyDownEvent(WPARAM wParam) {}

	virtual void Move(DWORD dwDirection);
	virtual void Jump() {}
	virtual void Trace(float elapsedTime);

	virtual void Attack();
	virtual void OnHit();
	virtual void OnDeath();
	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection);

protected:
	MonsterAnimationState m_AnimationState = MonsterAnimationState::Monster_State_Idle;

};



#endif // !MONSTER_H
