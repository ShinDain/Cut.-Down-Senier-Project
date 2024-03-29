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
		Player_State_Throw,
		Player_State_Act
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

	// 사운드 파일 이름
	const char* run1_SoundFileName = "Sound/Player/Footstep01.wav";
	const char* run2_SoundFileName = "Sound/Player/Footstep02.wav";
	const char* attack1_SoundFileName = "Sound/Player/Attack01.wav";
	const char* attack2_SoundFileName = "Sound/Player/Attack02.wav";
	const char* attack3_SoundFileName = "Sound/Player/Attack03.wav";
	const char* hit1_SoundFileName = "Sound/Player/Hit01.wav";
	const char* hit2_SoundFileName = "Sound/Player/Hit02.wav";
	const char* hit3_SoundFileName = "Sound/Player/Hit03.wav";
	const char* hit4_SoundFileName = "Sound/Player/Hit04.wav";
	const char* grab1_SoundFileName = "Sound/Player/Grab01.wav";		
	const char* grab2_SoundFileName = "Sound/Player/Grab02.wav";		
	const char* throw1_SoundFileName = "Sound/Player/Throw01.wav";		
	const char* throw2_SoundFileName = "Sound/Player/Throw02.wav";		
	const char* throw3_SoundFileName = "Sound/Player/Throw03.wav";		
	const char* throw4_SoundFileName = "Sound/Player/Throw04.wav";		
	const char* death1_SoundFileName = "Sound/Player/Death01.wav";		
	const char* death2_SoundFileName = "Sound/Player/Death02.wav";		

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

	virtual void CinematicAction();
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
	void CameraRayToMovableObject();

	std::shared_ptr<Object> m_pPickedObject = nullptr;
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


	void SetScore(UINT nScore) { m_nScore = nScore; }
	void SetHP(float nHP) { m_HP = nHP; }
	void SetWeapon(std::shared_ptr<Weapon> pWeapon) { m_pWeapon = pWeapon; }
	void SetCameraRotation(XMFLOAT3 xmf3CameraRotation) { m_xmf3CameraRotation = xmf3CameraRotation; }
	void SetCameraPosition(XMFLOAT3 xmf3CameraPosition) { m_xmf3CameraPosition = xmf3CameraPosition; }
	void SetPlayerTargetObject(std::shared_ptr<Object> pTargetObject) { m_pTargetObject = pTargetObject; }
};

#endif