//***************************************************************************************
// MathHelper.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "MathHelper.h"
#include <float.h>
#include <cmath>

using namespace DirectX;

const float MathHelper::Infinity = FLT_MAX;
const float MathHelper::Pi = 3.1415926535f;

float MathHelper::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// 제 1, 4 사분면
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * Pi; // in [0, 2*pi).
	}
	// 제 2, 3 사분면
	else
		theta = atanf(y / x) + Pi; // in [0 , 2 * pi).

	return theta;
}

XMVECTOR MathHelper::RandUnitVec3()
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f),
			MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// 균일한 분포를 얻기 위해 단위구 외부의 점은 배제한다.
		// 그렇지 않으면 큐브 모서리 점들이 뭉치게 될 것이다.
		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		return XMVector3Normalize(v);
	}
}

XMVECTOR MathHelper::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f),
			MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// 균일한 분포를 얻기 위해 단위구 외부의 점은 배제한다.
		// 그렇지 않으면 큐브 모서리 점들이 뭉치게 될 것이다.
		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		// 아래쪽 반구의 점은 무시한다.
		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}