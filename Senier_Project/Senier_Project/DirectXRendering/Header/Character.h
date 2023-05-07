#pragma once

#include "../../Common/Header/D3DUtil.h"
#include "../../Physics/Header/RigidBody.h"
#include "Object.h"

struct Ray
{
	XMFLOAT3 xmf3Start = XMFLOAT3(0, 0, 0);
	XMFLOAT3 xmf3Direction = XMFLOAT3(0, -1, 0);
	float length = 10.f;
};

// Object를 상속, State Machine을 가지며 애니메이션 전환이 가능
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

	bool m_bCanJump = true;

	Ray m_floorCheckRay;
	

public:
	virtual void Move(DWORD dwDirection, float distance);
	virtual void Rotate(float x, float y, float z);

	virtual void Jump();

	bool IsFalling();
};