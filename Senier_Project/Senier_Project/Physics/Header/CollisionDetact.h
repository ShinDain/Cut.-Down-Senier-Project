#pragma once

#include "../../Common/Header/D3DUtil.h"
#include "Collider.h"

#define MAX_CONTACT_CNT 256

class IntersectTests
{
public:
	static bool SphereAndHalfSpace(
		const ColliderSphere& sphere,
		const ColliderPlane& plane);

	static bool SphereAndSphere(
		const ColliderSphere& sphere1,
		const ColliderSphere& sphere2);

	static bool BoxAndHalfSpace(
		const ColliderBox& box,
		const ColliderPlane& plane);

	static bool BoxAndBox(
		const ColliderBox& box1,
		const ColliderBox& box2);

};