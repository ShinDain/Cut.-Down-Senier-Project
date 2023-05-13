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
