#pragma once

#include "Global.h"
#include "D3DUtil.h"

using namespace DirectX;

class Object;

class AnimationSet
{

};

class AnimationSets
{

};

class AnimationTrack
{

};

class AnimationController
{

};

class ModelDataInfo
{
public:
	ModelDataInfo() {}
	~ModelDataInfo();

	std::shared_ptr<Object> m_pRootObject = nullptr;

	int m_nSkinMeshes = 0;
	//std::shared_ptr<SkinMesh> m_pSkinMeshes = nullptr;

	std::shared_ptr<AnimationSets> m_pAnimationSets = nullptr;

public:
	void PrepareSkinning();

};
