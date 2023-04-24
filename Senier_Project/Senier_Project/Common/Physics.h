#pragma once

#include "D3DUtil.h"
#include "Object.h"

using namespace DirectX;

namespace Physics
{
	// SAT 충돌 체크 알고리즘
	bool SAT(Object* pObjectA, Object* pObjectB);

	std::vector<XMVECTOR> CalculateAxes(XMVECTOR* squareNormals, XMMATRIX objWorldA, XMMATRIX objWorldB);
	bool CheckIntersect(Object* pObjectA, Object* pObjectB, std::vector<XMVECTOR> vAxes, 
						XMMATRIX objWorldA, XMMATRIX objWorldB,
						XMFLOAT3& outCollisionNormal, float& outDepth);

	XMFLOAT3 CalculateCollisionPoint(Object* pObject, XMVECTOR collisionNormal, XMVECTOR* squareNormals);

	//=============================================================

	void CalculateImpulseAndApply(
		Object* pObjectA, Object* pObjectB,
		XMFLOAT3 xmf3CollisionNormal,
		XMFLOAT3 xmf3CollisionPointA, XMFLOAT3 xmf3CollisionPointB,
		float depth, float restitution);

	XMMATRIX CalculateContactMatrix(XMFLOAT3 xmf3CollisionNormal);

	float CalculateDeltaVelocityByUnitImpulse(
		Object* pObjectA, Object* pObjectB,
		XMFLOAT3 xmf3CollisionNormal,
		XMFLOAT3 xmf3CollisionPointA, XMFLOAT3 xmf3CollisionPointB);

	float CalculateDesiredDeltaVelocity(
		Object* pObjectA, Object* pObjectB,
		XMFLOAT3 xmf3CollisionNormal,
		XMFLOAT3 xmf3CollisionPointA, XMFLOAT3 xmf3CollisionPointB,
		float restitution);

}
