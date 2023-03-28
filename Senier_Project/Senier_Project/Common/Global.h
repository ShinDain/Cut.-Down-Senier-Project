#pragma once

// 정점 구조체 정의
// 상수 버퍼 구조체 정의

#include <tchar.h>
#include "MathHelper.h"
#include "UploadBuffer.h"

#define SKINNED_ANIMATION_BONES 256

struct ObjConstant
{
	DirectX::XMFLOAT4X4 World = MathHelper::identity4x4();
};

struct PassConstant
{
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::identity4x4();
};

struct MatConstant
{
	DirectX::XMFLOAT4 AlbedoColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct BoneBindPoseOffsetConstant
{
	DirectX::XMFLOAT4X4 BoneOffset[SKINNED_ANIMATION_BONES];
};
struct SkinningBoneTransformConstant
{
	DirectX::XMFLOAT4X4 BoneTransform[SKINNED_ANIMATION_BONES];
};

int ReadintegerFromFile(FILE* pInFile);
float ReadFloatFromFile(FILE* pInFile);
BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken);


