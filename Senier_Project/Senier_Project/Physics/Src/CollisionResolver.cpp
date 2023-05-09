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

	// Contact 데이터 사전 준비
	PrepareContacts(pContacts, elapsedTime);

	// 교차 해결을 위한 Position 조정
	AdjustPositions(pContacts, elapsedTime);

	// Collision으로 인한 Velocity 조정
	AdjustVelocities(pContacts, elapsedTime);
}

void CollisionResolver::PrepareContacts(std::vector<std::shared_ptr<Contact>> pContacts, float elapsedTime)
{
	// Contact를 순회하며 데이터 준비

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
		max = Physics::velocityEpsilon;
		index = pContacts.size();
		// 가장 (기대 속도 변화량)이 큰 Contact를 찾는다.
		for (int i = 0; i < pContacts.size(); ++i)
		{
			if (pContacts[i]->GetDesiredDeltaVelocity() > max)
			{
				max = pContacts[i]->GetDesiredDeltaVelocity();
				index = i;
			}
		}
		if (index == pContacts.size()) break;

		// 연산을 위해 관련 body Awake 
		pContacts[index]->MatchAwakeState();

		// Contact, 속도 변화 연산 및 적용
		pContacts[index]->ApplyVelocityChange(velocityDelta[0], velocityDelta[1], angularDelta[0], angularDelta[1]);

		// index Contact 속도 연산 적용 후 연관 Contact의 데이터 수정
		// 속도 변화로 인해 DesiredVelocity가 수정된다.

		for (int i = 0; i < 2; ++i)
		{
			if (pContacts[index]->GetBody(i))
			{
				std::vector<std::shared_ptr<Contact>> BodyContacts = pContacts[index]->GetBody(i)->GetRelativeContacts();

				for (int j = 0; j < BodyContacts.size(); ++j)
				{
					for (int k = 0; k < 2; ++k)
					{
						if (BodyContacts[j]->GetBody(k) == pContacts[index]->GetBody(i))
						{
							XMVECTOR relativePosition = XMLoadFloat3(&BodyContacts[j]->GetRelativeContactPosition(k));
							XMMATRIX worldToContact = XMLoadFloat4x4(&BodyContacts[j]->GetContactToWorld());
							worldToContact = XMMatrixTranspose(worldToContact);

							deltaVelocity = XMVector3Cross(angularDelta[i], relativePosition) + velocityDelta[i];
							XMVECTOR tmp = XMLoadFloat3(&BodyContacts[j]->GetContactVelocity());
							tmp += XMVector3TransformNormal(deltaVelocity, worldToContact) * (k ? -1 : 1);
							XMFLOAT3 xmf3Result;
							XMStoreFloat3(&xmf3Result, tmp);
							BodyContacts[j]->SetContactVelocity(xmf3Result);

							// ContactVelocity 수정 후 기대 속도 변화도 재연산
							BodyContacts[j]->CalcDesiredDeltaVelocity(elapsedTime);
						}
					}
				}
			}
		}
		
		//for (int i = 0; i < pContacts.size(); ++i)
		//{
		//	for (int b = 0; b < 2; ++b)
		//	{
		//		if (pContacts[i]->GetBody(b))
		//		{
		//			for (int d = 0; d < 2; ++d)
		//			{
		//				if (pContacts[i]->GetBody(b) == pContacts[index]->GetBody(d))
		//				{
		//					XMVECTOR relativePosition = XMLoadFloat3(&pContacts[i]->GetRelativeContactPosition(b));
		//					XMMATRIX worldToContact = XMLoadFloat4x4(&pContacts[i]->GetContactToWorld());
		//					worldToContact = XMMatrixTranspose(worldToContact);

		//					deltaVelocity = XMVector3Cross(angularDelta[d], relativePosition) + velocityDelta[d];
		//					XMVECTOR tmp = XMLoadFloat3(&pContacts[i]->GetContactVelocity());
		//					tmp += XMVector3TransformNormal(deltaVelocity, worldToContact) * (b ? -1 : 1);
		//					XMFLOAT3 xmf3Result;
		//					XMStoreFloat3(&xmf3Result, tmp);
		//					pContacts[i]->SetContactVelocity(xmf3Result);

		//					// ContactVelocity 수정 후 기대 속도 변화도 재연산
		//					pContacts[i]->CalcDesiredDeltaVelocity(elapsedTime);
		//				}
		//			}
		//		}
		//	}
		//}

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

	std::vector<std::shared_ptr<Contact>> orderedContacts = pContacts;
	
	//std::sort(orderedContacts.begin(), orderedContacts.end(), [](auto first, auto second)->bool
	//	{
	//		return first->GetDepth() > second->GetDepth();
	//	});

	m_nPositionIterationCnt = 0;
	while (m_nPositionIterationCnt < m_nIteration)
	{
		max = Physics::positionEpsilon;;
		index = pContacts.size();

		//if (orderedContacts[0]->GetDepth() < max)
		//	break;

		//for (int i = 0; i < orderedContacts.size(); ++i)
		//{
		//	// 가장 교차된 깊이가 큰 index를 탐색한다.
		//	if (orderedContacts[0]->GetDepth() < max)
		//	{
		//		max = orderedContacts[0]->GetDepth();
		//		index = i;
		//	}
		//}

		for (int i = 0; i < pContacts.size(); ++i)
		{
			// 가장 교차된 깊이가 큰 index를 탐색한다.
			if (pContacts[i]->GetDepth() > max)
			{
				max = pContacts[i]->GetDepth();
				index = i;
			}
		}

		if (index == pContacts.size()) break;

		// 연산을 위해 관련 body Awake 
		pContacts[index]->MatchAwakeState();

		// 교차 해결을 위한 위치 이동
		pContacts[index]->ApplyPositionChange(linearDelta[0], linearDelta[1], angularDelta[0], angularDelta[1], max);

		// 위치 이동이 일어난 이후 연관된 Contact 데이터도 수정되어야 한다.
		for (int i = 0; i < 2; ++i)
		{
			if (pContacts[index]->GetBody(i))
			{
				std::vector<std::shared_ptr<Contact>> BodyContacts = pContacts[index]->GetBody(i)->GetRelativeContacts();

				for (int j = 0; j < BodyContacts.size(); ++j)
				{
					for (int k = 0; k < 2; ++k)
					{
						if (BodyContacts[j]->GetBody(k) == pContacts[index]->GetBody(i))
						{
							XMVECTOR relativePosition = XMLoadFloat3(&BodyContacts[j]->GetRelativeContactPosition(k));

							// 이동 변화량
							deltaPosition = XMVector3Cross(angularDelta[i], relativePosition) + linearDelta[i];

							// 해당 Contact의 Contact Normal 방향으로 사영시킨 크기를 적용하여
							// Depth를 새롭게 구한다.
							XMVECTOR contactNormal = XMLoadFloat3(&BodyContacts[j]->GetContactNormal());
							float newDepth = BodyContacts[j]->GetDepth();
							newDepth += XMVectorGetX(XMVector3Dot(contactNormal, deltaPosition)) * (k ? 1 : -1);
							BodyContacts[j]->SetDepth(newDepth);
						}
					}
				}
			}
		}

		//for (int i = 0; i < pContacts.size(); ++i)
		//{
		//	for (int b = 0; b < 2; ++b)
		//	{
		//		if (pContacts[i]->GetBody(b))
		//		{
		//			for (int d = 0; d < 2; ++d)
		//			{
		//				if (pContacts[i]->GetBody(b) == pContacts[index]->GetBody(d))
		//				{
		//					
		//					XMVECTOR relativePosition = XMLoadFloat3(&pContacts[i]->GetRelativeContactPosition(b));

		//					// 이동 변화량
		//					deltaPosition = XMVector3Cross(angularDelta[d], relativePosition) + linearDelta[d];

		//					// 해당 Contact의 Contact Normal 방향으로 사영시킨 크기를 적용하여
		//					// Depth를 새롭게 구한다.
		//					XMVECTOR contactNormal = XMLoadFloat3(&pContacts[i]->GetContactNormal());
		//					float newDepth = pContacts[i]->GetDepth();
		//					newDepth += XMVectorGetX(XMVector3Dot(contactNormal, deltaPosition)) * (b ? 1:-1);
		//					pContacts[i]->SetDepth(newDepth);
		//				}
		//			}
		//		}
		//	}
		//}

		++m_nPositionIterationCnt;
	}



}
