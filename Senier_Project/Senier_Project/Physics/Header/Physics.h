#ifndef PHYSICS_H

#define PHYSICS_H

#include "../../Common/Header/D3DUtil.h"

using namespace DirectX;

namespace Physics
{
	static float sleepEpsilon = 0.3f;
	static float velocityEpsilon = 0.1f;
	static float positionEpsilon = 0.01f;
	static const XMFLOAT3 xmf3Gravity = XMFLOAT3(0, -9.81f, 0);
}

#endif // !