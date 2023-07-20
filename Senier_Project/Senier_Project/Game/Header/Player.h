#ifndef PLAYER_H

#define PLAYER_H

#include "../../Common/Header/D3DUtil.h"
#include "Character.h"
#include "Weapon.h"
#include "../../DirectXRendering/Header/AnimationController.h"

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

private:
	enum PlayerAnimationState
	{
		Player_State_Idle,
		Player_State_Jump,
		Player_State_Falling,
		Player_State_Land,
		Player_State_Melee,
		Player_State_Hit,
		Player_State_Death,
		Player_State_Throw
	};

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
		Player_Anim_Index_Death,
		Player_Anim_Index_ThrowIdle,
		Player_Anim_Index_Throw
	};

	// ���� ���� �̸�
	//LPCWSTR idle1_SoundFileName = L"";
	//LPCWSTR idle2_SoundFileName = L"";
	//LPCWSTR idle3_SoundFileName = L"";
	//LPCWSTR walk1_SoundFileName = L"Sound/Footstep01.wav";
	//LPCWSTR walk2_SoundFileName = L"Sound/Footstep02.wav";
	//LPCWSTR run1_SoundFileName = L"";
	//LPCWSTR run2_SoundFileName = L"";
	//LPCWSTR jump_SoundFileName = L"Sound/Evil_Machine_Loop_00.wav";
	//LPCWSTR doublejump_SoundFileName = L"Sound/123.wav";
	//LPCWSTR land_SoundFileName = L"Sound/Footstep04.wav";
	//LPCWSTR attack1_SoundFileName = L"Sound/123.wav";
	//LPCWSTR attack2_SoundFileName = L"Sound/Footstep04.wav";
	//LPCWSTR attack3_SoundFileName = L"Sound/Footstep03.wav";
	//LPCWSTR hit1_SoundFileName = L"Sound/123.wav";
	//LPCWSTR hit2_SoundFileName = L"Sound/Footstep04.wav";
	//LPCWSTR death1_SoundFileName = L"Sound/Footstep01.wav";
	//LPCWSTR death2_SoundFileName = L"Sound/Footstep02.wav";
	//LPCWSTR grab1_SoundFileName = L"Sound/Footstep01.wav";		// �׷�
	//LPCWSTR grab2_SoundFileName = L"Sound/Footstep02.wav";		// �׷�
	//LPCWSTR throw1_SoundFileName = L"Sound/123.wav";		// ��ô
	//LPCWSTR throw2_SoundFileName = L"Sound/123.wav";		// ��ô

	LPCWSTR idle1_SoundFileName = L"";
	LPCWSTR idle2_SoundFileName = L"";
	LPCWSTR idle3_SoundFileName = L"";
	LPCWSTR walk1_SoundFileName =  L"";
	LPCWSTR walk2_SoundFileName =  L"";
	LPCWSTR run1_SoundFileName = L"";
	LPCWSTR run2_SoundFileName = L"";
	LPCWSTR jump_SoundFileName = L"";
	LPCWSTR doublejump_SoundFileName = L"";
	LPCWSTR land_SoundFileName = L"";
	LPCWSTR attack1_SoundFileName =  L"";
	LPCWSTR attack2_SoundFileName =  L"";
	LPCWSTR attack3_SoundFileName =  L"";
	LPCWSTR hit1_SoundFileName = L"";
	LPCWSTR hit2_SoundFileName = L"";
	LPCWSTR death1_SoundFileName =  L"";
	LPCWSTR death2_SoundFileName =  L"";
	LPCWSTR grab1_SoundFileName =  L"";		// �׷�
	LPCWSTR grab2_SoundFileName =  L"";		// �׷�
	LPCWSTR throw1_SoundFileName =  L"";		// ��ô
	LPCWSTR throw2_SoundFileName =  L"";		// ��ô

public:
	virtual void ProcessInput(UCHAR* pKeybuffer);
	virtual void KeyDownEvent(WPARAM wParam);
	virtual void KeyUpEvent(WPARAM wParam);
	void LeftButtonDownEvent();
	void RightButtonDownEvent();

	virtual void Move(DWORD dwDirection);
	virtual void Jump();
	void ChangeToJumpState();

	virtual void Attack();
	virtual void ThrowProjectile();
	void RotateToObj();
	void AcquireItem(UINT itemType);

	void InitializeState();

	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection);

	virtual void DoLanding();

	virtual void UpdateAnimationTrack(float elapsedTime);
	virtual void BlendWithIdleMovement(float maxWeight);

protected:
	UINT m_nScore = 0;

	bool m_bIgnoreInput = false;

	std::shared_ptr<Weapon> m_pWeapon = nullptr;
	
	XMFLOAT3 m_xmf3CameraRotation = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3CameraPosition = XMFLOAT3(0, 0, 0);
	//XMFLOAT3 m_xmf3CameraLook = XMFLOAT3(0, 0, 0);
	//bool m_bCanDoubleJump = true;
	bool m_bCanThrow = true;

	float m_AimmingSpeed = 50.0f;
	//bool m_bSprint = false;
	//float m_SprintSpeed = 120.0f;
	//bool m_bDecreaseMaxSpeed = false;

	bool m_bDashToIdle = false;
	float m_TransitionTime = 0.5f;
	float m_ElapsedTransitionTime = 0.0f;

	UINT m_nMaxAttackCombo = 2;
	UINT m_nAttackCombo = 0;
	bool m_bCombeAttack = false;
	float m_AttackRange = 40.0f;
	float m_AttackAccel = 30.0f;

	UINT m_nCurAttackTrack = 0;
	UINT m_nNextAttackTrack = 0;

	PlayerAnimationState m_nAnimationState = PlayerAnimationState::Player_State_Idle;
	BoundingSphere m_ObjectSearchSphere;

	std::shared_ptr<Object> m_pTargetObject = nullptr;

protected:
	enum GrabState
	{
		Grab_Empty,
		Grab_Moving,
		Grab_Complete
	};

	void ObjectGrab();
	void UpdateGrabedObjectPosition(float elapsedTime);
	std::shared_ptr<Object> CameraRayToMovableObject(bool bCharacter, float& outDistance);

	std::shared_ptr<Object> m_pGrabedObject = nullptr;
	GrabState m_GrabState = GrabState::Grab_Empty;
	float m_GrappleTime = 10.0f;
	float m_ElapsedGrappleTime = 0.0f;

	float m_GrapNoiseRight = 0.0f;
	float m_GrapNoiseUp = 0.0f;

	XMFLOAT3 m_xmf3GrabedObjectStartPosition = XMFLOAT3(0, 0, 0);

	XMFLOAT3 m_xmf3GrabOffsetPosition = XMFLOAT3(10, 10, 4);
	
public:
	std::shared_ptr<Weapon> GetWeapon() { return m_pWeapon; }
	XMFLOAT3 GetCameraRotation() {	return m_xmf3CameraRotation;}
	UINT GetScore() { return m_nScore; }
	Object* GetPlayerTargetObject() { return m_pTargetObject.get(); }


	void SetWeapon(std::shared_ptr<Weapon> pWeapon) { m_pWeapon = pWeapon; }
	void SetCameraRotation(XMFLOAT3 xmf3CameraRotation) { m_xmf3CameraRotation = xmf3CameraRotation; }
	void SetCameraPosition(XMFLOAT3 xmf3CameraPosition) { m_xmf3CameraPosition = xmf3CameraPosition; }
	void SetPlayerTargetObject(std::shared_ptr<Object> pTargetObject) { m_pTargetObject = pTargetObject; }
};

#endif