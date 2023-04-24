#include "Physics.h"

bool Physics::SAT(Object* pObjectA, Object* pObjectB)
{
	// 두 충돌체 모두 육면체인 경우
	XMFLOAT4X4 xmf4x4ObjWorldA = pObjectA->GetWorld();
	XMFLOAT4X4 xmf4x4ObjWorldB = pObjectB->GetWorld();
	XMMATRIX objWorldA = XMLoadFloat4x4(&xmf4x4ObjWorldA);
	XMMATRIX objWorldB = XMLoadFloat4x4(&xmf4x4ObjWorldB);

	XMFLOAT3 xmf3SquareNormals[6] = { XMFLOAT3(1, 0, 0), XMFLOAT3(0, 1, 0),XMFLOAT3(0, 0, 1),
									XMFLOAT3(-1, 0, 0), XMFLOAT3(0, -1, 0),XMFLOAT3(0, 0, -1) };
	XMVECTOR squareNormals[6];
	for (int i = 0; i < 6; ++i)
	{
		squareNormals[i] = XMLoadFloat3(&xmf3SquareNormals[i]);
	}

	// 1. 분리축이 될 수 있는 후보 벡터들을 구한다.
	std::vector<XMVECTOR> vAxes = CalculateAxes(squareNormals, objWorldA, objWorldB);

	// 2. 구한 벡터들에 물체를 사영시킨 결과가 교차하는지 검증한다.
	// Collision Normal과 Depth를 획득
	XMFLOAT3 xmf3CollisionNormal = { 0,0,0 };
	float depth = 0;
	if (!CheckIntersect(pObjectA, pObjectB, vAxes, objWorldA, objWorldB, xmf3CollisionNormal, depth))
		return false;

	XMVECTOR collisionNormal = XMLoadFloat3(&xmf3CollisionNormal);

	// 3. Collision Point를 얻어야 한다.
	XMFLOAT3 xmf3CollisionPointA = CalculateCollisionPoint(pObjectA, collisionNormal, squareNormals);
	XMFLOAT3 xmf3CollisionPointB = CalculateCollisionPoint(pObjectB, -collisionNormal, squareNormals);
	
	// 4. 충돌 정보를 통해 충격량을 계산 후 물체에 적용
	CalculateImpulseAndApply(pObjectA, pObjectB, xmf3CollisionNormal, xmf3CollisionPointA, xmf3CollisionPointB, depth, 0);

	return true;
}

std::vector<XMVECTOR> Physics::CalculateAxes(XMVECTOR* squareNormals, XMMATRIX objWorldA, XMMATRIX objWorldB)
{
	std::vector<XMVECTOR> resultAxes;
	std::vector<XMVECTOR> vAxesA;
	std::vector<XMVECTOR> vAxesB;

	// 법선 벡터의 변환의 경우 월드 변환 행렬의 역행렬의 전치행렬이 필요하다.
	XMMATRIX transposeInverseWorldA = XMMatrixTranspose(XMMatrixInverse(nullptr, objWorldA));
	XMMATRIX transposeInverseWorldB = XMMatrixTranspose(XMMatrixInverse(nullptr, objWorldB));

	// 물체 A의 normal
	for (int i = 0; i < 3; ++i)
	{
		XMVECTOR tmp = XMVector3TransformNormal(squareNormals[i], transposeInverseWorldA);
		tmp = XMVector3Normalize(tmp);
		vAxesA.emplace_back(tmp);
		resultAxes.emplace_back(tmp);
	}
	// 물체 B의 normal
	for (int i = 0; i < 3; ++i)
	{
		XMVECTOR tmp = XMVector3TransformNormal(squareNormals[i], transposeInverseWorldB);
		tmp = XMVector3Normalize(tmp);
		vAxesB.emplace_back(tmp);
		resultAxes.emplace_back(tmp);
	}
	// 두 물체의 normal의 외적
	for (int i = 0; i < 3; ++i)
	{
		for (int k = 0; k < 3; ++k)
		{
			XMVECTOR tmp = XMVector3Cross(vAxesA[i], vAxesB[k]);
			tmp = XMVector3Normalize(tmp);
			if (XMVectorGetX(XMVector3Length(tmp)) < 1)
				continue;
			resultAxes.emplace_back(tmp);
		}
	}

	return resultAxes;
}

bool Physics::CheckIntersect(Object* pObjectA, Object* pObjectB, std::vector<XMVECTOR> vAxes, 
							XMMATRIX objWorldA, XMMATRIX objWorldB, 
							XMFLOAT3& outCollisionNormal, float& outDepth)
{
	std::vector<XMFLOAT3> xmf3VerticesA = pObjectA->m_pCollider->GetColliderVertices();
	std::vector<XMVECTOR> verticesA;
	for (XMFLOAT3 xmf3Va : xmf3VerticesA)
	{
		verticesA.emplace_back(XMVector3TransformCoord(XMLoadFloat3(&xmf3Va), objWorldA));
	}
	std::vector<XMFLOAT3> xmf3VerticesB = pObjectB->m_pCollider->GetColliderVertices();
	std::vector<XMVECTOR> verticesB;
	for (XMFLOAT3 xmf3Vb : xmf3VerticesB)
	{
		verticesB.emplace_back(XMVector3TransformCoord(XMLoadFloat3(&xmf3Vb), objWorldB));
	}

	float depth = FLT_MAX;						// Depth
	XMVECTOR smallestAxis = XMVectorZero();		// Collision Normal
	for (XMVECTOR axis : vAxes)
	{

		float minVa = FLT_MAX;
		float maxVa = FLT_MIN;
		float minVb = FLT_MAX;
		float maxVb = FLT_MIN;

		for (XMVECTOR va : verticesA)
		{
			float result = XMVectorGetX(XMVector3Dot(va, axis));
			minVa = min(minVa, result);
			maxVa = max(maxVa, result);
		}
		for (XMVECTOR vb : verticesB)
		{
			float result = XMVectorGetX(XMVector3Dot(vb, axis));
			minVb = min(minVb, result);
			maxVb = max(maxVb, result);
		}
		
		// 분리축 생성 가능
		if (maxVb < minVa || maxVa < minVb)
		{
			return false;
		}
		else
		{
			float length = FLT_MAX;
			if (maxVa < maxVb) // A - B
			{
				length = maxVa - minVb;
			}
			else			   // B - A
			{
				length = maxVb - minVa;
			}

			if (depth > length)
			{
				depth = length;
				smallestAxis = axis;
			}
		}
	}
	XMStoreFloat3(&outCollisionNormal, smallestAxis);
	outDepth = depth;

	return true;
}

XMFLOAT3 Physics::CalculateCollisionPoint(Object* pObject, XMVECTOR collisionNormal, XMVECTOR* squareNormals)
{
	float maxDot = 0;
	XMFLOAT3 xmf3CollisionFaceNormal = XMFLOAT3(0, 0, 0);
	for (int i = 0; i < 6; ++i)
	{
		float cmpRes = XMVectorGetX(XMVector3Dot(collisionNormal, squareNormals[i]));
		if (maxDot < cmpRes)
		{
			maxDot = cmpRes;
			XMStoreFloat3(&xmf3CollisionFaceNormal, squareNormals[i]);
		}
	}
	// 충돌 평면
	XMFLOAT3 xmf3Extents = pObject->m_pCollider->GetExtents();
	float a = xmf3CollisionFaceNormal.x * xmf3Extents.x;
	float b = xmf3CollisionFaceNormal.y * xmf3Extents.y;
	float c = xmf3CollisionFaceNormal.z * xmf3Extents.z;
	float d = -(a + b + c);

	XMVECTOR collisionFaceNormal = XMLoadFloat3(&xmf3CollisionFaceNormal);

	float t = XMVectorGetX(XMVector3Dot(collisionFaceNormal, collisionNormal));
	t = -1 * d / t;

	XMFLOAT3 xmf3CollisionNormal;
	XMStoreFloat3(&xmf3CollisionNormal, collisionNormal);	// Collision Normal
	XMFLOAT3 xmf3CollisionPoint = XMFLOAT3(t * xmf3CollisionNormal.x, t * xmf3CollisionNormal.y, t * xmf3CollisionNormal.z);

	return xmf3CollisionPoint;
}

//========================================================================

void Physics::CalculateImpulseAndApply(
	Object* pObjectA, Object* pObjectB,
	XMFLOAT3 xmf3CollisionNormal,
	XMFLOAT3 xmf3CollisionPointA, XMFLOAT3 xmf3CollisionPointB,
	float depth, float restitution)
{
	// 1. 단위 충격량당 속도 변화 계산
	float deltaVelocityByUnitImpulse = CalculateDeltaVelocityByUnitImpulse(pObjectA, pObjectB, xmf3CollisionNormal, 
																			xmf3CollisionPointA, xmf3CollisionPointB);

	// 2. 충돌 후 기대되는 속도 변화량 계산, 오브젝트의 각속도도 선속도로 통합시켜주어야 한다.
	float desiredDeltaVelocity = CalculateDesiredDeltaVelocity(pObjectA, pObjectB, xmf3CollisionNormal,
																xmf3CollisionPointA, xmf3CollisionPointB, restitution);

	// 3. 기대 속도 변화 / 단위 충격량당 속도 변화를 계산하여 각 물체에 적용될 충격량 계산
	XMMATRIX xmmatContactToWorld = CalculateContactMatrix(xmf3CollisionNormal);

	XMFLOAT3 xmf3Impulse = XMFLOAT3(desiredDeltaVelocity / deltaVelocityByUnitImpulse, 0, 0);
	// Contact 좌표계를 World 좌표게로 변환
	XMVECTOR impulse = XMVector3TransformCoord(XMLoadFloat3(&xmf3Impulse), xmmatContactToWorld);

	// 4. Object에 Impulse 적용
#if defined(_DEBUG)
	/*XMFLOAT3 deltaPosition;
	XMStoreFloat3(&deltaPosition, XMLoadFloat3(&xmf3CollisionNormal) * depth);
	pObjectA->AddPosition(deltaPosition);

	XMStoreFloat3(&deltaPosition, -1 * XMLoadFloat3(&xmf3CollisionNormal) * depth);
	pObjectB->AddPosition(deltaPosition);*/
#endif


	XMStoreFloat3(&xmf3Impulse, impulse);
	pObjectA->Impulse(xmf3Impulse, xmf3CollisionNormal, xmf3CollisionPointA);
	impulse = -impulse;
	XMStoreFloat3(&xmf3Impulse, impulse);
	pObjectB->Impulse(xmf3Impulse, xmf3CollisionNormal, xmf3CollisionPointB);
}

XMMATRIX Physics::CalculateContactMatrix(XMFLOAT3 xmf3CollisionNormal)
{
	XMFLOAT3 contactTangent[2];

	if (fabsf(xmf3CollisionNormal.x) > fabsf(xmf3CollisionNormal.y))
	{
		float s = 1.f / sqrtf(xmf3CollisionNormal.z * xmf3CollisionNormal.z + xmf3CollisionNormal.x * xmf3CollisionNormal.x);

		contactTangent[0].x = xmf3CollisionNormal.z * s;
		contactTangent[0].y = 0.f;
		contactTangent[0].z = -xmf3CollisionNormal.x * s;

		contactTangent[1].x = xmf3CollisionNormal.y * contactTangent[0].z;
		contactTangent[1].y = xmf3CollisionNormal.z * contactTangent[0].x - xmf3CollisionNormal.x * contactTangent[0].z;
		contactTangent[1].z = -xmf3CollisionNormal.y * contactTangent[0].x;
	}
	else
	{
		float s = 1.f / sqrtf(xmf3CollisionNormal.y * xmf3CollisionNormal.y + xmf3CollisionNormal.z * xmf3CollisionNormal.z);

		contactTangent[0].x = 0.f;
		contactTangent[0].y = -xmf3CollisionNormal.z * s;
		contactTangent[0].z = xmf3CollisionNormal.y * s;

		contactTangent[1].x = xmf3CollisionNormal.y * contactTangent[0].z - xmf3CollisionNormal.z * contactTangent[0].y;
		contactTangent[1].y = -xmf3CollisionNormal.x * contactTangent[0].z;
		contactTangent[1].z = xmf3CollisionNormal.x * contactTangent[0].y;
	}

	XMFLOAT3X3 xmf3x3ContactToWorld = XMFLOAT3X3(xmf3CollisionNormal.x, xmf3CollisionNormal.y, xmf3CollisionNormal.z,
												 contactTangent[0].x, contactTangent[0].y, contactTangent[0].z,
												 contactTangent[1].x, contactTangent[1].y, contactTangent[1].z);

	return XMLoadFloat3x3(&xmf3x3ContactToWorld);
}

float Physics::CalculateDeltaVelocityByUnitImpulse(Object* pObjectA, Object* pObjectB, XMFLOAT3 xmf3CollisionNormal,
													XMFLOAT3 xmf3CollisionPointA, XMFLOAT3 xmf3CollisionPointB)
{
	XMVECTOR collisionNormal = XMLoadFloat3(&xmf3CollisionNormal);
	// 물체 A의 단위 충격량당 속도 변화량
	XMVECTOR collisionPointA = XMLoadFloat3(&xmf3CollisionPointA);
	XMMATRIX inverseRotateInertia = XMMatrixInverse(nullptr, pObjectA->GetRotateInertia());
	XMVECTOR deltaVelWorld = XMVector3Cross(collisionPointA, collisionNormal);
	deltaVelWorld = XMVector3TransformNormal(deltaVelWorld, inverseRotateInertia);
	deltaVelWorld = XMVector3Cross(deltaVelWorld, collisionPointA);

	// 선속도로 통합하기 위해 충돌 노멀에 정사영시킨다.
	float deltaVelocityByUnitImpulse = XMVectorGetX(XMVector3Dot(deltaVelWorld, collisionNormal));
	deltaVelocityByUnitImpulse += (1 / pObjectA->GetMass());

	// 물체 B의 단위 충격량당 속도 변화량
	XMVECTOR collisionPointB = XMLoadFloat3(&xmf3CollisionPointB);
	inverseRotateInertia = XMMatrixInverse(nullptr, pObjectB->GetRotateInertia());
	deltaVelWorld = XMVector3Cross(collisionPointB, collisionNormal);
	deltaVelWorld = XMVector3TransformNormal(deltaVelWorld, inverseRotateInertia);
	deltaVelWorld = XMVector3Cross(deltaVelWorld, collisionPointB);

	deltaVelocityByUnitImpulse += XMVectorGetX(XMVector3Dot(deltaVelWorld, collisionNormal));
	deltaVelocityByUnitImpulse += (1 / pObjectB->GetMass());

	return deltaVelocityByUnitImpulse;
}

float Physics::CalculateDesiredDeltaVelocity(Object* pObjectA, Object* pObjectB, XMFLOAT3 xmf3CollisionNormal,
											 XMFLOAT3 xmf3CollisionPointA, XMFLOAT3 xmf3CollisionPointB, float restitution)
{
	XMVECTOR collisionPointA = XMLoadFloat3(&xmf3CollisionPointA);
	XMVECTOR collisionPointB = XMLoadFloat3(&xmf3CollisionPointB);

	XMVECTOR velocityA = XMLoadFloat3(&pObjectA->GetVelocity());
	XMVECTOR velocityB = XMLoadFloat3(&pObjectB->GetVelocity());
	XMVECTOR angleVelocityA = XMLoadFloat3(&pObjectA->GetAngleVelocity());
	XMVECTOR angleVelocityB = XMLoadFloat3(&pObjectB->GetAngleVelocity());

	XMMATRIX xmmatContactToWorld = CalculateContactMatrix(xmf3CollisionNormal);

	// 물체 A
	XMVECTOR contactVelWorldA = XMVector3Cross(angleVelocityA, collisionPointA);
	contactVelWorldA += velocityA;
	XMVECTOR contactVelA = XMVector3TransformNormal(contactVelWorldA, XMMatrixTranspose(xmmatContactToWorld));

	// 물체 B
	XMVECTOR contactVelWorldB = XMVector3Cross(angleVelocityB, collisionPointB);
	contactVelWorldB += velocityB;
	XMVECTOR contactVelB = XMVector3TransformNormal(contactVelWorldB, XMMatrixTranspose(xmmatContactToWorld));

	// 분리 속도
	XMVECTOR contactVelocity = contactVelA - contactVelB;
	XMFLOAT3 xmf3ContactVelocity = XMFLOAT3(0, 0, 0);
	XMStoreFloat3(&xmf3ContactVelocity, contactVelocity);
	float desiredDeltaVelocity = -xmf3ContactVelocity.x - restitution * (xmf3ContactVelocity.x);

	return desiredDeltaVelocity;
}
