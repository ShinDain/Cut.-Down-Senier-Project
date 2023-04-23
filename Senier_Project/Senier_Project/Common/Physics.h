#pragma once

#include "D3DUtil.h"
#include "Object.h"

using namespace DirectX;

namespace Physics
{
	// SAT �浹 üũ �˰���



	void CalculateImpulse(
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
