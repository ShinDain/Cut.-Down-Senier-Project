#include "Physics.h"

void Physics::CalculateImpulse(
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
	XMStoreFloat3(&xmf3Impulse, impulse);

	// 4. Object에 Impulse 적용
	pObjectA->Impulse(xmf3Impulse, xmf3CollisionNormal, xmf3CollisionPointA);
	pObjectA->Impulse(xmf3Impulse, xmf3CollisionNormal, xmf3CollisionPointB);
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
