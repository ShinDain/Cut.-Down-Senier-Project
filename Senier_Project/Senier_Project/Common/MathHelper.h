//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>
#include <float.h>
#include <cmath>

using namespace DirectX;

namespace MathHelper
{
	const float Infinity = FLT_MAX;
	const float Pi = 3.1415926535f;

	// [0, 1) 사이의 무작위 float 값 반환
	inline float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// [a, b) 사이의 무작위 float 값 반환
	inline float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	// [a, b) 사이의 무작위 정수 반환
	inline int Rand(int a, int b)
	{
		return a + rand() % ((b - a) + 1);
	}

	template<typename T>
	inline T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	inline T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	inline bool IsZero(const T& a, const T& epsilon)
	{
		return fabsf(a) < epsilon;
	}

	template<typename T>
	inline bool IsEqual(const T& a, const T& b, const T& epsilon)
	{
		return IsZero((a - b), epsilon);
	}

	template<typename T>
	inline T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

	template<typename T>
	inline T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	// 
	inline float AngleFromXY(float x, float y)
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
	 
	inline XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return XMVectorSet(
			radius * sinf(phi) * cosf(theta),
			radius * cosf(phi),
			radius * sinf(phi) * sinf(theta),
			1.0f);
	}

	inline XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		// 노멀값에만 적용시킬 역전치 행렬을 반환, 
		// 일반적인 변환에 사용하지 않도록 주의
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

	inline XMFLOAT4X4 identity4x4()
	{
		static XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}

	inline XMFLOAT4X4 Zero4x4()
	{
		static XMFLOAT4X4 I(
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);

		return I;
	}

	inline XMVECTOR RandUnitVec3()
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

	inline XMVECTOR RandHemisphereUnitVec3(XMVECTOR n)
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

	inline XMFLOAT4X4 XMFloat4x4Interpolate(XMFLOAT4X4& xmf4x4Matrix1, XMFLOAT4X4& xmf4x4Matrix2, float t)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMVECTOR S0, R0, T0, S1, R1, T1;
		XMMatrixDecompose(&S0, &R0, &T0, XMLoadFloat4x4(&xmf4x4Matrix1));
		XMMatrixDecompose(&S1, &R1, &T1, XMLoadFloat4x4(&xmf4x4Matrix2));
		XMVECTOR S = XMVectorLerp(S0, S1, t);
		XMVECTOR T = XMVectorLerp(T0, T1, t);
		XMVECTOR R = XMQuaternionSlerp(R0, R1, t);
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
		return (xmf4x4Result);
	}

}


