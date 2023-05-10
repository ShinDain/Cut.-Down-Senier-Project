#pragma once

#include "../../Common/Header/D3DUtil.h"
#include "Object.h"

struct Ray
{
	XMFLOAT3 xmf3Start = XMFLOAT3(0, 0, 0);
	XMFLOAT3 xmf3Direction = XMFLOAT3(0, -1, 0);
	float length = 10.f;
};

// Object�� ���, State Machine�� ������ �ִϸ��̼� ��ȯ�� ����
class Character : public Object
{
public:
	Character();
	Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ObjectInitData objData,
		std::shared_ptr<ModelDataInfo> pModel, int nAnimationTracks);

	Character(const Character& rhs) = delete;
	Character& operator=(const Character& rhs) = delete;
	virtual ~Character();

	virtual void Update(float elapsedTime);

protected:
	float m_MaxSpeedXZ = 100.0f;
	float m_CharacterFriction = 350.0f;
	float m_JumpSpeed = 25.0f;

	Ray m_floorCheckRay;

public:
	virtual void Move(DWORD dwDirection);
	virtual void Jump();
	virtual void IsFalling();

	void ApplyCharacterFriction(float elapsedTime);
	void CalcVelocityAndPosition(float elapsedTime);
};