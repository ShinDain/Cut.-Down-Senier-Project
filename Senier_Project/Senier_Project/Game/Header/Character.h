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
	virtual void Attack() {}

	virtual void IsFalling();

	virtual void DoLanding();
	virtual	void RotateToMove(float elapsedTime);
	virtual	void RotateToTargetLook(float elapsedTime, XMFLOAT3 xmf3TargetLook, float divideConst);

	virtual void BlendWithIdleMovement(float maxWeight);

protected:
	float m_MaxSpeedXZ = 100.0f;
	float m_CharacterFriction = 350.0f;
	float m_JumpSpeed = 100.0f;

	Ray m_floorCheckRay;

	float m_TurnSpeed = 1;

	void ApplyCharacterFriction(float elapsedTime);
	void CalcVelocityAndPosition(float elapsedTime);
};