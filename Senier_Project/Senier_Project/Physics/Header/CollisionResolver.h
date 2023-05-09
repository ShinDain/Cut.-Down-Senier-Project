#pragma once

#include <algorithm>

#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/MathHelper.h"
#include "Contact.h"

class CollisionResolver
{
public:
	CollisionResolver() = delete;
	CollisionResolver(unsigned int Iteration);
	CollisionResolver(const CollisionResolver& rhs) = delete;
	CollisionResolver& operator=(const CollisionResolver& rhs) = delete;
	virtual ~CollisionResolver();

protected:
	// 반복 횟수
	unsigned int m_nIteration = 0;

public:
	unsigned int m_nVelocityIterationCnt = 0;
	unsigned int m_nPositionIterationCnt = 0;

public:
	// 외부에서 호출, Contacts 데이터 해결
	void ResolveContacts(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime);

protected:
	// 충돌 연산 이전, Contact 데이터를 사전 준비
	void PrepareContacts(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime);
	// 속도 해결부
	void AdjustVelocities(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime);
	// 교차 해결부
	void AdjustPositions(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime);

public:
	bool IsValid()
	{
		return (m_nVelocityIterationCnt < m_nIteration) && (m_nPositionIterationCnt < m_nIteration);
	}

	void SetVelocityIterationCnt(unsigned int VelocityIterationCnt) {m_nVelocityIterationCnt = VelocityIterationCnt;}
	void SetPositionIterationCnt(unsigned int PositionIterationCnt) { m_nPositionIterationCnt = PositionIterationCnt; }

	const unsigned int GetVelocityIterationCnt() { return m_nVelocityIterationCnt; }
	const unsigned int GetPositionIterationCnt() { return m_nPositionIterationCnt; }
};

 