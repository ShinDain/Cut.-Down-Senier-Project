#pragma once

#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Object.h"

#define CHARACTER_IDLE_TRACK 0
#define CHARACTER_MOVE_TRACK 1

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

	virtual void InitAnimationTrack(float animationSpeed) {}

	virtual void Update(float elapsedTime);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void ProcessInput(UINT msg, UCHAR* pKeybuffer) {}
	virtual void KeyDownEvent(WPARAM wParam) {}

	virtual void UpdateAnimationTrack(float elapsedTime) {}
	virtual void UnableAnimationTrack(int nAnimationTrack);

	void SpawnBloodEffect(UINT nCnt);

public:
	virtual void Move(DWORD dwDirection) {}
	virtual void Jump() {}

	virtual void IsFalling();

	virtual void DoLanding();
	virtual	void RotateToMove(float elapsedTime);
	virtual	void RotateToTargetLook(float elapsedTime, XMFLOAT3 xmf3TargetLook, float divideConst);

	virtual void ApplyDamage(float power, XMFLOAT3 xmf3DamageDirection, XMFLOAT3 xmf3CuttingDirection);
	virtual void Cutting(XMFLOAT3 xmf3PlaneNormal);

	virtual void BlendWithIdleMovement(float maxWeight);
	virtual void BlendIdleToAnimaiton(float curTrackRate, float goalRate, float mul, UINT nTrackIdx);
	virtual void BlendAnimationToIdle(float curTrackRate, float startRate, float mul, UINT nTrackIdx);
	virtual void BlendAnimationToAnimation(float curTrackRate, float startRate, float mul, UINT nTrackIdx1, UINT nTrackIdx2);

protected:
	float m_DefaultAccel = 500.f;
	float m_DefaultMaxSpeedXZ = 100.f;
	float m_DefaultFriction = 350.0f;

	float m_AnimationSpeed = 1.0f;

	float m_MaxSpeedXZ = 100.0f;
	float m_CharacterFriction = 350.0f;
	float m_JumpSpeed = 100.0f;

	float m_TurnSpeed = 1;

	bool m_bIsShoulderView = false;

	void ApplyCharacterFriction(float elapsedTime);

	XMFLOAT3 m_xmf3CharacterMovement = XMFLOAT3(0, 0, 0);

public:
	virtual void MoveForward() {
		XMVECTOR characterMovement = XMLoadFloat3(&m_xmf3Look);
		characterMovement *= m_DefaultAccel;

		XMStoreFloat3(&m_xmf3CharacterMovement, characterMovement);
	}
	virtual void WalkForward() 
	{
		XMVECTOR characterMovement = XMLoadFloat3(&m_xmf3Look);
		characterMovement *= 355.0f;

		XMStoreFloat3(&m_xmf3CharacterMovement, characterMovement);

		m_pBody->SetVelocity(m_xmf3CharacterMovement);
	}
	virtual void MoveStop() {
		m_xmf3CharacterMovement = XMFLOAT3(0, 0, 0);
	}

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
