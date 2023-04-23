#include "Physics.h"

void Physics::CalculateImpulse(
	Object* pObjectA, Object* pObjectB,
	XMFLOAT3 xmf3CollisionNormal,
	XMFLOAT3 xmf3CollisionPointA, XMFLOAT3 xmf3CollisionPointB,
	float depth, float restitution)
{
	// 1. ���� ��ݷ��� �ӵ� ��ȭ ���
	float deltaVelocityByUnitImpulse = CalculateDeltaVelocityByUnitImpulse(pObjectA, pObjectB, xmf3CollisionNormal, 
																			xmf3CollisionPointA, xmf3CollisionPointB);

	// 2. �浹 �� ���Ǵ� �ӵ� ��ȭ�� ���, ������Ʈ�� ���ӵ��� ���ӵ��� ���ս����־�� �Ѵ�.
	float desiredDeltaVelocity = CalculateDesiredDeltaVelocity(pObjectA, pObjectB, xmf3CollisionNormal,
																xmf3CollisionPointA, xmf3CollisionPointB, restitution);

	// 3. ��� �ӵ� ��ȭ / ���� ��ݷ��� �ӵ� ��ȭ�� ����Ͽ� �� ��ü�� ����� ��ݷ� ���
	XMMATRIX xmmatContactToWorld = CalculateContactMatrix(xmf3CollisionNormal);

	XMFLOAT3 xmf3Impulse = XMFLOAT3(desiredDeltaVelocity / deltaVelocityByUnitImpulse, 0, 0);
	// Contact ��ǥ�踦 World ��ǥ�Է� ��ȯ
	XMVECTOR impulse = XMVector3TransformCoord(XMLoadFloat3(&xmf3Impulse), xmmatContactToWorld);
	XMStoreFloat3(&xmf3Impulse, impulse);

	// 4. Object�� Impulse ����
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
	// ��ü A�� ���� ��ݷ��� �ӵ� ��ȭ��
	XMVECTOR collisionPointA = XMLoadFloat3(&xmf3CollisionPointA);
	XMMATRIX inverseRotateInertia = XMMatrixInverse(nullptr, pObjectA->GetRotateInertia());
	XMVECTOR deltaVelWorld = XMVector3Cross(collisionPointA, collisionNormal);
	deltaVelWorld = XMVector3TransformNormal(deltaVelWorld, inverseRotateInertia);
	deltaVelWorld = XMVector3Cross(deltaVelWorld, collisionPointA);

	// ���ӵ��� �����ϱ� ���� �浹 ��ֿ� ���翵��Ų��.
	float deltaVelocityByUnitImpulse = XMVectorGetX(XMVector3Dot(deltaVelWorld, collisionNormal));
	deltaVelocityByUnitImpulse += (1 / pObjectA->GetMass());

	// ��ü B�� ���� ��ݷ��� �ӵ� ��ȭ��
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

	// ��ü A
	XMVECTOR contactVelWorldA = XMVector3Cross(angleVelocityA, collisionPointA);
	contactVelWorldA += velocityA;
	XMVECTOR contactVelA = XMVector3TransformNormal(contactVelWorldA, XMMatrixTranspose(xmmatContactToWorld));

	// ��ü B
	XMVECTOR contactVelWorldB = XMVector3Cross(angleVelocityB, collisionPointB);
	contactVelWorldB += velocityB;
	XMVECTOR contactVelB = XMVector3TransformNormal(contactVelWorldB, XMMatrixTranspose(xmmatContactToWorld));

	// �и� �ӵ�
	XMVECTOR contactVelocity = contactVelA - contactVelB;
	XMFLOAT3 xmf3ContactVelocity = XMFLOAT3(0, 0, 0);
	XMStoreFloat3(&xmf3ContactVelocity, contactVelocity);
	float desiredDeltaVelocity = -xmf3ContactVelocity.x - restitution * (xmf3ContactVelocity.x);

	return desiredDeltaVelocity;
}
