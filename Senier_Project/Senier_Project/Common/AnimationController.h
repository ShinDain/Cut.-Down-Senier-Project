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

	std::shared_ptr<Object> mpRootObject = nullptr;

	int mnSkinMeshes = 0;
	//std::shared_ptr<SkinMesh> mpSkinMeshes = nullptr;

	std::shared_ptr<AnimationSets> mpAnimationSets = nullptr;

public:
	void PrepareSkinning();

};
