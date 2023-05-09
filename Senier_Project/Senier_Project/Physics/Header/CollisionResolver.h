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
	// �ݺ� Ƚ��
	unsigned int m_nIteration = 0;

public:
	unsigned int m_nVelocityIterationCnt = 0;
	unsigned int m_nPositionIterationCnt = 0;

public:
	// �ܺο��� ȣ��, Contacts ������ �ذ�
	void ResolveContacts(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime);

protected:
	// �浹 ���� ����, Contact �����͸� ���� �غ�
	void PrepareContacts(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime);
	// �ӵ� �ذ��
	void AdjustVelocities(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime);
	// ���� �ذ��
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

 