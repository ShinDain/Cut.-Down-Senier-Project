#pragma once

#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/MathHelper.h"
#include "RigidBody.h"

class Contact
{
public:
	Contact() = delete;
	Contact(RigidBody* pBody1, RigidBody* pBody2, float friction, float restitution,
		XMFLOAT3 xmf3ContactPoint, XMFLOAT3 xmf3ContactNormal, float depth);
	Contact(const Contact& rhs) = delete;
	Contact& operator=(const Contact& rhs) = delete;
	virtual ~Contact();

protected:
	RigidBody* m_pBody[2];

	float m_Friction = 0;								// 마찰력
	float m_Restitution = 0;							// 탄성 계수

	XMFLOAT3 m_xmf3ContactPoint = XMFLOAT3(0, 0, 0);	// 충돌점
	XMFLOAT3 m_xmf3ContactNormal = XMFLOAT3(0, 0, 0);	// 충돌의 방향 벡터

	float m_Depth = 0;									// 교차 깊이

protected:
	// Contact 좌표계에서 World 좌표계로의 변환 행렬
	XMFLOAT4X4 m_xmf4x4ContactToWorld = MathHelper::identity4x4();

	XMFLOAT3 m_xmf3ContactVelocity = XMFLOAT3(0, 0, 0);
	float m_DesiredDeltaVelocity = 0;

	// World 좌표계 기준
	XMFLOAT3 m_pxmf3RelativePosition[2];

public:
	// Resolve 수행 전 데이터 사전 연산 (Prepare)
	void CalcInternals(float elapsedTime);

	// 필요한 속도/위치 변화를 계산하여 body에 적용
	void ApplyVelocityChange(XMVECTOR& deltaLinearVel_0, XMVECTOR& deltaLinearVel_1,
							 XMVECTOR& deltaAngularVel_0, XMVECTOR& deltaAngularVel_1);
	void ApplyPositionChange(XMVECTOR& deltaLinearVel_0, XMVECTOR& deltaLinearVel_1, 
							 XMVECTOR& deltaAngularVel_0, XMVECTOR& deltaAngularVel_1, float depth);

	// Contact 발생한 Body의 상태를 동기화
	// Sleep인 경우 연산이 생략되므로 
	void MatchAwakeState();

	// DesiredDeltaVelocity를 계산, 충격량 계산 과정에 필요하다.
	void CalcDesiredDeltaVelocity(float elapsedTime);

protected:
	// 충돌은 0 -> 1을 순서로 진행, 벽/바닥과의 충돌의 경우 NULL일 수 있으므로
	// 순서를 교체
	void SwapBodies();

	// 충돌점으로의 접근속도를 반환
	XMVECTOR CalcLocalVelocity(int nbodyidx, float elapsedTime);

	// ContactToWorld 변환 행렬 계산
	void CalcContactToWorld();

	// 충돌 해결을 위해 필요한 Impulse의 크기 계산
	// 속도 변화를 구하기 위해 필요하다.
	XMVECTOR CalcFrictionlessImpulse(FXMMATRIX InverseInertia_0, CXMMATRIX InverseInertia_1);
	XMVECTOR CalcFrictionImpulse(FXMMATRIX InverseInertia_0, CXMMATRIX InverseInertia_1);

	
public:
	void SetFriction(float friction) { m_Friction = friction; };
	void SetRestitution(float restitution) { m_Restitution = restitution; };

	void SetContactPoint(XMFLOAT3 xmf3ContactPoint) { m_xmf3ContactPoint = xmf3ContactPoint; };
	void SetContactNormal(XMFLOAT3 xmf3ContactNormal) { m_xmf3ContactNormal = xmf3ContactNormal; };
	void SetContactVelocity(XMFLOAT3 xmf3ContactVelocity) { m_xmf3ContactVelocity = m_xmf3ContactVelocity;} 

	void SetDepth(float depth) { m_Depth = depth; }


		
	RigidBody* GetBody(int idx) { return m_pBody[idx]; }
	const float GetFriction() { return m_Friction; };
	const float GetRestitution() { return m_Restitution; };
	const XMFLOAT3& GetContactPoint() { return m_xmf3ContactPoint; };
	const XMFLOAT3& GetContactNormal() { return m_xmf3ContactNormal; };

	const float GetDepth() {return m_Depth; }

	const XMFLOAT4X4& GetContactToWorld() { return m_xmf4x4ContactToWorld; };

	const XMFLOAT3& GetContactVelocity() { return m_xmf3ContactVelocity; }
	const float GetDesiredDeltaVelocity() { return m_DesiredDeltaVelocity; }
	const XMFLOAT3& GetRelativeContactPosition(unsigned int idx) { return m_pxmf3RelativePosition[idx]; }
};

struct CollisionData
{
	std::vector<std::shared_ptr<Contact>> pContacts;

	int maxContacts = 256;

	float friction = 0.9f;

	float restitution = 0.1f;

	float tolerance = 0.1f;

	int ContactCnt()
	{
		return pContacts.size();
	}

	void Reset(int max)
	{
		maxContacts = max;
		pContacts.clear();
	}

	void addContact(RigidBody* pBody1, RigidBody* pBody2, float friction, float restitution,
		XMFLOAT3 xmf3ContactPoint, XMFLOAT3 xmf3ContactNormal, float depth)
	{
		pContacts.emplace_back(std::make_unique<Contact>(pBody1, pBody2, friction, restitution,
														xmf3ContactPoint, xmf3ContactNormal, depth));
	}
};