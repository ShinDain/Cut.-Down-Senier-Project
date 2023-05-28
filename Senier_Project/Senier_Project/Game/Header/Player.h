#ifndef PLAYER_H

#define PLAYER_H

#include "../../Common/Header/D3DUtil.h"
#include "Character.h"
#include "Weapon.h"
#include "../../DirectXRendering/Header/AnimationController.h"

enum PlayerAnimationIndex
{
	Player_Anim_Index_Idle,
	Player_Anim_Index_RunForward,
	Player_Anim_Index_RunBackward,
	Player_Anim_Index_RunLeft,
	Player_Anim_Index_RunRight,
	Player_Anim_Index_Sprint,
	Player_Anim_Index_StrafeLeft,
	Player_Anim_Index_StrafeRight,
	Player_Anim_Index_JumpUp,
	Player_Anim_Index_JumpDown,
	Player_Anim_Index_JumpWhileRunning,
	Player_Anim_Index_Falling,
	Player_Anim_Index_IdleCombat,
	Player_Anim_Index_MeleeOneHand,
	Player_Anim_Index_MeleeTwoHand,
	Player_Anim_Index_GetHit,
	Player_Anim_Index_Death
};

enum PlayerAnimationState
{
	Player_State_Idle,
	Player_State_Jump,
	Player_State_Falling,
	Player_State_Land,
	Player_State_Melee
};


class Player : public Character
{
public:
	Player();
	Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Player(const Player& rhs) = delete;
	Player& operator=(const Player& rhs) = delete;
	virtual ~Player();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Update(float elapsedTime);

	virtual void Destroy();

public:
	virtual void ProcessInput(UCHAR* pKeybuffer);
	virtual void KeyDownEvent(WPARAM wParam);

	virtual void Move(DWORD dwDirection);
	virtual void Jump();
	void ChangeToJumpState();
	void BlendWithIdleMovement(float maxWeight);
	
	virtual void Attack();
	virtual void OnHit();
	virtual void OnDeath();

	virtual void DoLanding();

	void UpdateAnimationTrack();
	void UnableAnimationTrack(int nAnimationTrack);

protected:
	std::shared_ptr<Weapon> m_pWeapon = nullptr;
	
	XMFLOAT3 m_xmf3CameraRotation = XMFLOAT3(0, 0, 0);
	bool m_bCanDoubleJump = true;

	PlayerAnimationState m_nAnimationState = PlayerAnimationState::Player_State_Idle;

public:
	void RotateToMove(float elapsedTime);


	std::shared_ptr<Weapon> GetWeapon() { return m_pWeapon; }
	XMFLOAT3 GetCameraRotation() {	return m_xmf3CameraRotation;}

	void SetWeapon(std::shared_ptr<Weapon> pWeapon) { m_pWeapon = pWeapon; }
	void SetCameraRotation(XMFLOAT3 xmf3CameraRotation) { m_xmf3CameraRotation = xmf3CameraRotation; }

};

class AttackCallbackHandler :public AnimationCallbackHandler
{
public:
	AttackCallbackHandler() {}
	virtual ~AttackCallbackHandler() {}

public:
	virtual void HandleCallback(void* pCallbackData, float TrackPosition);
};







#endif