#include "../Header/CollisionDetact.h"

float TransformToAxis(const ColliderBox& box, FXMVECTOR direction)
{
	float projectLength = 0;

	XMFLOAT3 boxExtent = box.GetExtents();

	// Extent X
	XMVECTOR Axis_0 = box.GetAxis(0);
	projectLength += fabs(XMVectorGetX(boxExtent.x * XMVector3Dot(Axis_0, direction)));

	// Extent Y
	XMVECTOR Axis_1 = box.GetAxis(1);
	projectLength += fabs(XMVectorGetX(boxExtent.y * XMVector3Dot(Axis_1, direction)));

	// Extent Z
	XMVECTOR Axis_2 = box.GetAxis(2);
	projectLength += fabs(XMVectorGetX(boxExtent.z * XMVector3Dot(Axis_2, direction)));

	return projectLength;
}

bool OverlapOnAxis(
	const ColliderBox& box1,
	const ColliderBox& box2,
	FXMVECTOR axis,
	FXMVECTOR toCentre
)
{
	float project_1 = TransformToAxis(box1, axis);
	float project_2 = TransformToAxis(box2, axis);

	float distance = fabs(XMVectorGetX(XMVector3Dot(toCentre, axis)));

	return (distance <= project_1 + project_2);
}

float PenetrationOnAxis(
	const ColliderBox& box1,
	const ColliderBox& box2,
	const FXMVECTOR axis,
	const FXMVECTOR toCentre
)
{
	float project_1 = TransformToAxis(box1, axis);
	float project_2 = TransformToAxis(box2, axis);

	float distance = fabs(XMVectorGetX(XMVector3Dot(toCentre, axis)));

	return (project_1 + project_2 - distance);
}

bool tryAxis(
	const ColliderBox& box1,
	const ColliderBox& box2,
	const FXMVECTOR axis,
	const FXMVECTOR toCentre,
	int index,

	float& smallestPenetration,
	int& smallestCase
)
{
	if (XMVectorGetX(XMVectorSum(axis * axis)) < 0.0001f)
		return true;
	XMVECTOR axisDir = XMVector3Normalize(axis);

	float penetration = PenetrationOnAxis(box1, box2, axisDir, toCentre);

	if (penetration < 0) return false;
	if (penetration <= smallestPenetration)
	{
		smallestPenetration = penetration;
		smallestCase = index;
	}
	return true;
}

void fillPointFaceBoxBox(
	const ColliderBox& box1,
	const ColliderBox& box2,
	const FXMVECTOR toCentre,
	CollisionData& pData,
	int best,
	float depth
)
{
	XMVECTOR normal = box1.GetAxis(best);
	normal = XMVector3Normalize(normal);
	if (XMVectorGetX(XMVector3Dot(normal, toCentre)) > 0)
	{
		normal = -normal;
	}

	XMFLOAT3 xmf3ContactPoint = box2.GetExtents();
	if (XMVectorGetX(XMVector3Dot(box2.GetAxis(0), normal)) < 0) xmf3ContactPoint.x = -xmf3ContactPoint.x;
	if (XMVectorGetX(XMVector3Dot(box2.GetAxis(1), normal)) < 0) xmf3ContactPoint.y = -xmf3ContactPoint.y;
	if (XMVectorGetX(XMVector3Dot(box2.GetAxis(2), normal)) < 0) xmf3ContactPoint.z = -xmf3ContactPoint.z;


	XMMATRIX world_2 = XMLoadFloat4x4(&box2.GetWorld());
	XMVECTOR contactPoint = XMLoadFloat3(&xmf3ContactPoint);
	contactPoint = XMVector3TransformCoord(contactPoint, world_2);
	XMStoreFloat3(&xmf3ContactPoint, contactPoint);

	XMFLOAT3 xmf3ContactNormal;
	XMStoreFloat3(&xmf3ContactNormal, normal);

	// 물리 연산을 하지 않는 경우 NULL로 전달
	std::shared_ptr<RigidBody> pBody1 = box1.GetBody();
	std::shared_ptr<RigidBody> pBody2 = box2.GetBody();
	if (!pBody1->GetPhysics())
		pBody1 = nullptr;
	if (!pBody2->GetPhysics())
		pBody2 = nullptr;

	assert(pBody1 != nullptr || pBody2 != nullptr);

	pData.addContact(pBody1, pBody2, pData.friction, pData.restitution, xmf3ContactPoint, xmf3ContactNormal, depth);
}

XMFLOAT3 CalcEdgeContactPoint(
	const FXMVECTOR& pLhs,
	const FXMVECTOR& dLhs,
	float lhsSize,
	const FXMVECTOR& pRhs,
	const CXMVECTOR& dRhs,
	float rhsSize,

	// Contact Point가 모서리 위에 없는 경우,
	// One의 중간점을 사용,
	bool useLhs
)
{
	XMFLOAT3 xmf3ContactPoint;

	float smLhs = XMVectorGetX(XMVector3LengthSq(dLhs));
	float smRhs = XMVectorGetX(XMVector3LengthSq(dRhs));
	float dpLhsRhs = XMVectorGetX(XMVector3Dot(dLhs, dRhs));

	XMVECTOR toSt = pLhs - pRhs;
	float dpStaLhs = XMVectorGetX(XMVector3Dot(toSt, dLhs));
	float dpStaRhs = XMVectorGetX(XMVector3Dot(toSt, dRhs));

	float denom = smLhs * smRhs - dpLhsRhs * dpLhsRhs;

	if (fabsf(denom) < 0.0001f)
	{
		XMStoreFloat3(&xmf3ContactPoint, useLhs ? pLhs : pRhs);

		return xmf3ContactPoint;
	}

	float  mua = (dpLhsRhs * dpStaRhs - smRhs * dpStaLhs) / denom;
	float  mub = (smLhs * dpStaRhs - dpLhsRhs * dpStaLhs) / denom;

	if (mua > lhsSize ||
		mua < -lhsSize ||
		mub > rhsSize ||
		mub < -rhsSize)
	{
		XMStoreFloat3(&xmf3ContactPoint, useLhs ? pLhs : pRhs);

		return xmf3ContactPoint;
	}
	else
	{
		XMVECTOR cLhs = pLhs + (dLhs * mua);
		XMVECTOR cRhs = pRhs + (dRhs * mub);

		XMStoreFloat3(&xmf3ContactPoint, (cLhs + cRhs) * 0.5);

		return xmf3ContactPoint;
	}
}


// =============== Intersect ============================

bool IntersectTests::SphereAndHalfSpace(const ColliderSphere& sphere, const ColliderPlane& plane)
{
	// 보류

	return false;
}

bool IntersectTests::SphereAndSphere(const ColliderSphere& sphere1, const ColliderSphere& sphere2)
{
	// 보류

	return false;
}

bool IntersectTests::BoxAndHalfSpace(const ColliderBox& box, const ColliderPlane& plane)
{
	// 박스의 Extent를 평면 법선벡터에 사영
	XMVECTOR direction = XMLoadFloat3(&plane.GetDirection());
	float projectLength = TransformToAxis(box, direction);

	// 평면과 박스 사이의 거리 획득
	XMVECTOR boxPosition = box.GetAxis(3);
	float boxDistance = XMVectorGetX(XMVector3Dot(direction, boxPosition)) - projectLength;

	return boxDistance <= plane.GetDistance();
}

bool IntersectTests::BoxAndBox(const ColliderBox& box1, const ColliderBox& box2)
{
	XMFLOAT4X4 xmf4x4World_1 = box1.GetWorld();
	XMFLOAT4X4 xmf4x4World_2 = box2.GetWorld();

	XMFLOAT3 xmf3temp = XMFLOAT3(xmf4x4World_1._41, xmf4x4World_1._42, xmf4x4World_1._43);
	XMVECTOR Position_1 = XMLoadFloat3(&xmf3temp);
	xmf3temp = XMFLOAT3(xmf4x4World_2._41, xmf4x4World_2._42, xmf4x4World_2._43);
	XMVECTOR Position_2 = XMLoadFloat3(&xmf3temp);

	XMVECTOR toCentre = Position_2 - Position_1;

	if (!OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(0), box2.GetAxis(0)), toCentre))
	{
		float tmp = 100;

	}

	// SAT 과정
	return {
		OverlapOnAxis(box1, box2, box1.GetAxis(0), toCentre) &&
		OverlapOnAxis(box1, box2, box1.GetAxis(1), toCentre) &&
		OverlapOnAxis(box1, box2, box1.GetAxis(2), toCentre) &&

		OverlapOnAxis(box1, box2, box2.GetAxis(0), toCentre) &&
		OverlapOnAxis(box1, box2, box2.GetAxis(1), toCentre) &&
		OverlapOnAxis(box1, box2, box2.GetAxis(2), toCentre) &&

		OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(0),box2.GetAxis(0)), toCentre) &&
		OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(0),box2.GetAxis(1)), toCentre) &&
		OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(0),box2.GetAxis(2)), toCentre) &&
		OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(1),box2.GetAxis(0)), toCentre) &&
		OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(1),box2.GetAxis(1)), toCentre) &&
		OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(1),box2.GetAxis(2)), toCentre) &&
		OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(2),box2.GetAxis(0)), toCentre) &&
		OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(2),box2.GetAxis(1)), toCentre) &&
		OverlapOnAxis(box1, box2, XMVector3Cross(box1.GetAxis(2),box2.GetAxis(2)), toCentre)
	};
}

// =============== Collision Detect ============================

int CollisionDetector::SphereAndHalfSpace(const ColliderSphere& sphere, const ColliderPlane& plane, CollisionData& pData)
{
	// 보류

	return 0;
}

int CollisionDetector::SphereAndTruePlane(const ColliderSphere& sphere, const ColliderPlane& plane, CollisionData& pData)
{
	// 보류

	return 0;
}

int CollisionDetector::SphereAndSphere(const ColliderSphere& sphere1, const ColliderSphere& sphere2, CollisionData& pData)
{
	// 보류

	return 0;
}

int CollisionDetector::BoxAndHalfSpace(const ColliderBox& box, const ColliderPlane& plane, CollisionData& pData)
{
	// 박스가 물리 연산을 하지 않는 경우 return;
	if (!box.GetPhysics() || box.GetBody()->GetInvalid())
		return 0;

	if (!box.GetBody()->GetIsAwake())
		return 0;

	if (pData.ContactCnt() > pData.maxContacts) return 0;

	if (!IntersectTests::BoxAndHalfSpace(box, plane)) return 0;

	static float vertices[8][3] = { {1,1,1},{-1,1,1},{1,-1,1},{-1,-1,1},
							   {1,1,-1},{-1,1,-1},{1,-1,-1},{-1,-1,-1} };

	XMVECTOR planeNormal = XMLoadFloat3(&plane.GetDirection());

	int contactCnt = 0;
	for (int i = 0; i < 8; ++i)
	{
		XMFLOAT3 xmf3VertexPos = XMFLOAT3(vertices[i][0], vertices[i][1], vertices[i][2]);
		XMVECTOR vertexPos = XMLoadFloat3(&xmf3VertexPos);
		XMVECTOR boxExtent = XMLoadFloat3(&box.GetExtents());
		vertexPos = XMVectorMultiply(vertexPos, boxExtent);

		XMMATRIX world = XMLoadFloat4x4(&box.GetWorld());
		vertexPos = XMVector3TransformCoord(vertexPos, world);

		float distance = XMVectorGetX(XMVector3Dot(vertexPos, planeNormal));

		if (distance <= plane.GetDistance())
		{
			XMVECTOR contactPoint = planeNormal;
			contactPoint = contactPoint * (distance - plane.GetDistance());
			contactPoint += vertexPos;
			XMFLOAT3 xmf3ContactPoint;
			XMStoreFloat3(&xmf3ContactPoint, contactPoint);

			XMFLOAT3 xmf3ContactNormal = plane.GetDirection();
			float depth = plane.GetDistance() - distance;

			pData.addContact(box.GetBody(), nullptr, pData.friction, pData.restitution, xmf3ContactPoint, xmf3ContactNormal, depth);

			++contactCnt;

			if (pData.ContactCnt() > pData.maxContacts) return contactCnt;
		}
	}

	return contactCnt;
}

int CollisionDetector::BoxAndBox(const ColliderBox& box1, const ColliderBox& box2, CollisionData& pData)
{
	if (box1.GetBody()->GetInvalid() || box2.GetBody()->GetInvalid())
		return 0;

	if (!box1.GetBody()->GetIsAwake() && !box2.GetBody()->GetIsAwake())
		return 0;

	// 두 물체 모두 물리 연산을 하지 않는 경우 return;
	if (!box1.GetPhysics() && !box2.GetPhysics())
		return 0;

	XMVECTOR toCentre = box2.GetAxis(3) - box1.GetAxis(3);

	if (XMVectorGetX(XMVectorIsNaN(toCentre)))
		return 0;

	float pen = FLT_MAX;
	int best = 0xffffff;

	// SAT 과정, penetration이 가장 작은 경우를 탐색 => 최소 이동을 위해 
	if (!tryAxis(box1, box2, box1.GetAxis(0), toCentre, 0, pen, best)) return 0;
	if (!tryAxis(box1, box2, box1.GetAxis(1), toCentre, 1, pen, best)) return 0;
	if (!tryAxis(box1, box2, box1.GetAxis(2), toCentre, 2, pen, best)) return 0;

	if (!tryAxis(box1, box2, box2.GetAxis(0), toCentre, 3, pen, best)) return 0;
	if (!tryAxis(box1, box2, box2.GetAxis(1), toCentre, 4, pen, best)) return 0;
	if (!tryAxis(box1, box2, box2.GetAxis(2), toCentre, 5, pen, best)) return 0;

	int bestSingleAxis = best;

	if (!tryAxis(box1, box2, XMVector3Cross(box1.GetAxis(0), box2.GetAxis(0)), toCentre, 6, pen, best)) return 0;
	if (!tryAxis(box1, box2, XMVector3Cross(box1.GetAxis(0), box2.GetAxis(1)), toCentre, 7, pen, best)) return 0;
	if (!tryAxis(box1, box2, XMVector3Cross(box1.GetAxis(0), box2.GetAxis(2)), toCentre, 8, pen, best)) return 0;
	if (!tryAxis(box1, box2, XMVector3Cross(box1.GetAxis(1), box2.GetAxis(0)), toCentre, 9, pen, best)) return 0;
	if (!tryAxis(box1, box2, XMVector3Cross(box1.GetAxis(1), box2.GetAxis(1)), toCentre, 10, pen, best)) return 0;
	if (!tryAxis(box1, box2, XMVector3Cross(box1.GetAxis(1), box2.GetAxis(2)), toCentre, 11, pen, best)) return 0;
	if (!tryAxis(box1, box2, XMVector3Cross(box1.GetAxis(2), box2.GetAxis(0)), toCentre, 12, pen, best)) return 0;
	if (!tryAxis(box1, box2, XMVector3Cross(box1.GetAxis(2), box2.GetAxis(1)), toCentre, 13, pen, best)) return 0;
	if (!tryAxis(box1, box2, XMVector3Cross(box1.GetAxis(2), box2.GetAxis(2)), toCentre, 14, pen, best)) return 0;

	assert(best != 0xffffff);

	if (best < 3)
	{
		fillPointFaceBoxBox(box1, box2, toCentre, pData, best, pen);
		return 1;
	}
	else if (best < 6)
	{
		// 반대 방향
		fillPointFaceBoxBox(box2, box1, -toCentre, pData, best - 3, pen);
		return 1;
	}
	else
	{
		// 모서리끼리 접촉한 경우

		best -= 6;
		int axisIndex_1 = best / 3;
		int axisIndex_2 = best % 3;
		XMVECTOR axis_1 = box1.GetAxis(axisIndex_1);
		XMVECTOR axis_2 = box2.GetAxis(axisIndex_2);
		XMVECTOR axis = XMVector3Cross(axis_1, axis_2);
		axis = XMVector3Normalize(axis);

		// box1 -> box2 방향이도록 (?)
		// 아무래도 오타인듯? box2에서 box1 방향으로 수정,
		// axis는 Contact Normal이므로 일관되게 box2에서 box1 방향이다.
		// box1 -> box2 방향이라는 의미가 축의 방향이 아닌듯
		if (XMVectorGetX(XMVector3Dot(axis, toCentre)) > 0) axis = -axis;

		XMFLOAT3 xmf3Extent_1 = box1.GetExtents();
		XMFLOAT3 xmf3Extent_2 = box2.GetExtents();
		float pExtents_1[3] = { xmf3Extent_1.x,xmf3Extent_1.y, xmf3Extent_1.z };
		float pExtents_2[3] = { xmf3Extent_2.x,xmf3Extent_2.y, xmf3Extent_2.z };

		for (int i = 0; i < 3; ++i)
		{
			if (i == axisIndex_1) pExtents_1[i] = 0;
			else if (XMVectorGetX(XMVector3Dot(box1.GetAxis(i), axis)) > 0) pExtents_1[i] = -pExtents_1[i];

			if (i == axisIndex_2) pExtents_2[i] = 0;
			else if (XMVectorGetX(XMVector3Dot(box2.GetAxis(i), axis)) < 0) pExtents_2[i] = -pExtents_2[i];
		}

		XMFLOAT3 xmf3PtEdge_1 = XMFLOAT3(pExtents_1[0], pExtents_1[1], pExtents_1[2]);
		XMFLOAT3 xmf3PtEdge_2 = XMFLOAT3(pExtents_2[0], pExtents_2[1], pExtents_2[2]);

		XMMATRIX world_1 = XMLoadFloat4x4(&box1.GetWorld());
		XMVECTOR ptOnEdge_1 = XMLoadFloat3(&xmf3PtEdge_1);
		ptOnEdge_1 = XMVector3TransformCoord(ptOnEdge_1, world_1);

		XMMATRIX world_2 = XMLoadFloat4x4(&box2.GetWorld());
		XMVECTOR ptOnEdge_2 = XMLoadFloat3(&xmf3PtEdge_2);
		ptOnEdge_2 = XMVector3TransformCoord(ptOnEdge_2, world_2);


		pExtents_1[0] = xmf3Extent_1.x; pExtents_1[1] = xmf3Extent_1.y; pExtents_1[2] = xmf3Extent_1.z;
		pExtents_2[0] = xmf3Extent_2.x; pExtents_2[1] = xmf3Extent_2.y; pExtents_2[2] = xmf3Extent_2.z;

		// 모서리 위 한 점을 Contact Point로 획득
		XMFLOAT3 xmf3ContactPoint = CalcEdgeContactPoint(
			ptOnEdge_1, axis_1, pExtents_1[axisIndex_1],
			ptOnEdge_2, axis_2, pExtents_2[axisIndex_2],
			bestSingleAxis > 2);

		// ===============================================================

		axis = XMVector3Normalize(axis);
		XMFLOAT3 xmf3ContactNormal;
		XMStoreFloat3(&xmf3ContactNormal, axis);

		// 물리 연산을 하지 않는 경우 NULL로 전달
		std::shared_ptr<RigidBody> pBody1 = box1.GetBody();
		std::shared_ptr<RigidBody> pBody2 = box2.GetBody();
		if (!pBody1->GetPhysics())
			pBody1 = nullptr;
		if (!pBody2->GetPhysics())
			pBody2 = nullptr;

		assert(pBody1 != nullptr || pBody2 != nullptr);

		pData.addContact(pBody1, pBody2, pData.friction, pData.restitution, xmf3ContactPoint, xmf3ContactNormal, pen);
	}


	return 0;
}

int CollisionDetector::BoxAndSphere(const ColliderBox& box, const ColliderSphere& sphere, CollisionData& pData)
{
	// 보류

	return 0;
}
