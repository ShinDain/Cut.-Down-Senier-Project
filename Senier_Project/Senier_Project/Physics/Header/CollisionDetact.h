#pragma once

#include "../../Common/Header/D3DUtil.h"
#include "Collider.h"
#include "Contact.h"

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

class CollisionDetector
{
public:
	static int SphereAndHalfSpace(
		const ColliderSphere& sphere,
		const ColliderPlane& plane,
		CollisionData& pData);

	static int SphereAndTruePlane(
		const ColliderSphere& sphere,
		const ColliderPlane& plane,
		CollisionData& pData);

	static int SphereAndSphere(
		const ColliderSphere& sphere1,
		const ColliderSphere& sphere2,
		CollisionData& pData);



	static int BoxAndHalfSpace(
		const ColliderBox& box,
		const ColliderPlane& plane,
		CollisionData& pData);

	static int BoxAndBox(
		const ColliderBox& box1,
		const ColliderBox& box2,
		CollisionData& pData);

	static int BoxAndSphere(
		const ColliderBox& box,
		const ColliderSphere& sphere,
		CollisionData& pData);

};