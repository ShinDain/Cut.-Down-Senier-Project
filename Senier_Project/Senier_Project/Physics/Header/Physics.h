#ifndef PHYSICS

#define PHYSICS


#include "../../Common/Header/D3DUtil.h"

using namespace DirectX;

namespace Physics
{
	static float sleepEpsilon = 0.3f;
	static const XMFLOAT3 xmf3Gravity = XMFLOAT3(0, -9.81f, 0);
}

#endif // !