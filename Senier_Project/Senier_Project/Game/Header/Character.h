#pragma once

#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Object.h"

#define CHARACTER_IDLE_TRACK 0
#define CHARACTER_MOVE_TRACK 1

struct Ray
{
	XMFLOAT3 xmf3Start = XMFLOAT3(0, 0, 0);
	XMFLOAT3 xmf3Direction = XMFLOAT3(0, -1, 0);
	float length = 10.f;
};

// Object를 상속, 애니메이션
class Character : public Object
{
public:
	Character();
	Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
			  ObjectInitData objData,
			  std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	Character(const Character& rhs) = delete;
	Character& operator=(const Character& rhs) = delete;
	virtual ~Character();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
							ObjectInitData objData,
							std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Update(float elapsedTime);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void ProcessInput(UINT msg, UCHAR* pKeybuffer) {}
	virtual void KeyDownEvent(WPARAM wParam) {}

	virtual void UpdateAnimationTrack(float elapsedTime) {}
	virtual void UnableAnimationTrack(int nAnimationTrack);

public:
	virtual void Move(DWORD dwDirection) {}
	virtual void Jump() {}

	virtual void IsFalling();

	virtual void DoLanding();
	virtual	void RotateToMove(float elapsedTime);
	virtual	void RotateToTargetLook(float elapsedTime, XMFLOAT3 xmf3TargetLook, float divideConst);

	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection);
	virtual void Cutting(XMFLOAT3 xmf3PlaneNormal);

	virtual void BlendWithIdleMovement(float maxWeight);

protected:
	float m_DefaultAccel = 500.f;
	float m_DefaultMaxSpeedXZ = 100.f;

	float m_AnimationSpeed = 1.0f;

	float m_MaxSpeedXZ = 100.0f;
	float m_CharacterFriction = 350.0f;
	float m_JumpSpeed = 100.0f;

	float m_TurnSpeed = 1;

	bool m_bCrashWithObject = false;
	float m_CrashPower = 0;
	XMFLOAT3 m_xmf3CrashDirection = { 0,0,0 };

	bool m_bIsShoulderView = false;

	void ApplyCharacterFriction(float elapsedTime);
	void CalcVelocityAndPosition(float elapsedTime);

public:
	void CrashWithObject(float crashPower, XMFLOAT3 xmf3CrashDirection);

	void SetCrashWithObject(bool bCrashWithObject) { m_bCrashWithObject = bCrashWithObject; }
	void SetCrashPower(float crashPower) { m_CrashPower = crashPower; }
	void SetCrashDirection(XMFLOAT3 xmf3CrashDirection) { m_xmf3CrashDirection = xmf3CrashDirection; }
	void SetIsShoulderView(bool bIsShoulderView) { m_bIsShoulderView = bIsShoulderView; }
};


class AnimTestCharacter : public Character
{
public:
	AnimTestCharacter() {};
	AnimTestCharacter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	AnimTestCharacter(const AnimTestCharacter& rhs) = delete;
	AnimTestCharacter& operator=(const AnimTestCharacter& rhs) = delete;
	virtual ~AnimTestCharacter();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks, void* pContext);

	virtual void Update(float elapsedTime);
	virtual void KeyDownEvent(WPARAM wParam);

protected:
	UINT m_tmpAnimationIdx = 0;

};
