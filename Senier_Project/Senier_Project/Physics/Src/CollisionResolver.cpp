#include "../Header/CollisionResolver.h"

CollisionResolver::CollisionResolver(unsigned int Iteration) : m_nIteration(Iteration)
{
}

CollisionResolver::~CollisionResolver()
{
}

void CollisionResolver::ResolveContacts(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime)
{
	if (pContacts.size() == 0) return;
	//if (!IsValid()) return;

	// Contact ������ ���� �غ�
	PrepareContacts(pContacts, elapsedTime);

	// ���� �ذ��� ���� Position ����
	AdjustPositions(pContacts, elapsedTime);

	// Collision���� ���� Velocity ����
	//AdjustVelocities(pContacts, elapsedTime);
}

void CollisionResolver::PrepareContacts(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime)
{
	// Contact�� ��ȸ�ϸ� ������ �غ�

	for (auto iter = pContacts.begin(); iter != pContacts.end(); ++iter)
	{
		Contact* thisContact = iter->get();

		thisContact->CalcInternals(elapsedTime);
	}
}

void CollisionResolver::AdjustVelocities(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime)
{
	float max;
	int index;

	XMVECTOR velocityDelta[2];
	XMVECTOR angularDelta[2];
	XMVECTOR deltaVelocity = XMVectorZero();

	m_nVelocityIterationCnt = 0;
	while (m_nVelocityIterationCnt < m_nIteration)
	{
		max = VELOCITY_EPSILON;
		index = pContacts.size();
		// ���� (��� �ӵ� ��ȭ��)�� ū Contact�� ã�´�.
		for (int i = 0; i < pContacts.size(); ++i)
		{
			if (pContacts[i]->GetDesiredDeltaVelocity() > max)
			{
				max = pContacts[i]->GetDesiredDeltaVelocity();
				index = i;
			}
		}
		if (index == pContacts.size()) break;

		// ������ ���� ���� body Awake 
		pContacts[index]->MatchAwakeState();

		// Contact, �ӵ� ��ȭ ���� �� ����
		pContacts[index]->ApplyVelocityChange(velocityDelta[0], velocityDelta[1], angularDelta[0], angularDelta[1]);

		// index Contact �ӵ� ���� ���� �� ���� Contact�� ������ ����
		// �ӵ� ��ȭ�� ���� DesiredVelocity�� �����ȴ�.
		// ��� Contact�� ��ȸ	(���� ����)
		for (int i = 0; i < pContacts.size(); ++i)
		{
			for (int b = 0; b < 2; ++b)
			{
				if (pContacts[i]->GetBody(b))
				{
					for (int d = 0; d < 2; ++d)
					{
						if (pContacts[i]->GetBody(b) == pContacts[index]->GetBody(d))
						{
							XMVECTOR relativePosition = XMLoadFloat3(&pContacts[i]->GetRelativeContactPosition(b));

							// World ��ǥ�� ���� �ӵ� ��ȭ��
							deltaVelocity = velocityDelta[d] + XMVector3Cross(angularDelta[d], relativePosition);

							XMMATRIX worldToContact = XMLoadFloat4x4(&pContacts[i]->GetContactToWorld());
							worldToContact = XMMatrixTranspose(worldToContact);
							deltaVelocity = XMVector3TransformNormal(deltaVelocity, worldToContact) * (b ? -1 : 1);

							XMVECTOR bodyContactVelocity = XMLoadFloat3(&pContacts[i]->GetContactVelocity());
							bodyContactVelocity += deltaVelocity;


							XMFLOAT3 xmf3ResultContactVelocity;
							XMStoreFloat3(&xmf3ResultContactVelocity, bodyContactVelocity);

							pContacts[i]->SetContactVelocity(xmf3ResultContactVelocity);

							// ContactVelocity ���� �� ��� �ӵ� ��ȭ�� �翬��
							pContacts[i]->CalcDesiredDeltaVelocity(elapsedTime);
						}
					}
				}
			}
		}

		++m_nVelocityIterationCnt;
	}
}

void CollisionResolver::AdjustPositions(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime)
{
	float max;
	int index;
	XMVECTOR linearDelta[2];
	XMVECTOR angularDelta[2];
	XMVECTOR deltaPosition;

	m_nPositionIterationCnt = 0;
	while (m_nPositionIterationCnt < m_nIteration)
	{
		max = POSITION_EPSILON;
		index = pContacts.size();
		for (int i = 0; i < pContacts.size(); ++i)
		{
			// ���� ������ ���̰� ū index�� Ž���Ѵ�.
			if (pContacts[i]->GetDepth() > max)
			{
				max = pContacts[i]->GetDepth();
				index = i;
			}
		}
		if (index == pContacts.size()) break;

		// ������ ���� ���� body Awake 
		pContacts[index]->MatchAwakeState();

		// ���� �ذ��� ���� ��ġ �̵�
		pContacts[index]->ApplyPositionChange(linearDelta[0], linearDelta[1], angularDelta[0], angularDelta[1], max);

		// ��ġ �̵��� �Ͼ ���� ������ Contact �����͵� �����Ǿ�� �Ѵ�.
		// ��� Contact�� ��ȸ	(���� ����)
		for (int i = 0; i < pContacts.size(); ++i)
		{
			for (int b = 0; b < 2; ++b)
			{
				if (pContacts[i]->GetBody(b))
				{
					for (int d = 0; d < 2; ++d)
					{
						if (pContacts[i]->GetBody(b) == pContacts[index]->GetBody(d))
						{
							
							XMVECTOR relativePosition = XMLoadFloat3(&pContacts[i]->GetRelativeContactPosition(b));

							// �̵� ��ȭ��
							deltaPosition = XMVector3Cross(angularDelta[d], relativePosition) + linearDelta[d];

							// �ش� Contact�� Contact Normal �������� �翵��Ų ũ�⸦ �����Ͽ�
							// Depth�� ���Ӱ� ���Ѵ�.
							XMVECTOR contactNormal = XMLoadFloat3(&pContacts[i]->GetContactNormal());
							float newDepth = pContacts[i]->GetDepth();
							newDepth += XMVectorGetX(XMVector3Dot(contactNormal, deltaPosition)) * (b ? 1:-1);
							pContacts[i]->SetDepth(newDepth);
						}
					}
				}
			}
		}

		++m_nPositionIterationCnt;
	}



}
