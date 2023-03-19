#pragma once

#include "Global.h"
#include "D3DUtil.h"

#define ANIMATION_TYPE_ONCE				0
#define ANIMATION_TYPE_LOOP				1
#define ANIMATION_TYPE_PINGPONG			2

#define ANIMATION_CALLBACK_EPSILON	0.00165f

using namespace DirectX;

class Object;
class SkinnedMesh;

struct CALLBACKKEY
{
	float m_Time = 0.0f;
	void* m_pCallbackData = NULL;
};

class AnimationCallbackHandler
{
public:
	AnimationCallbackHandler() {}
	virtual ~AnimationCallbackHandler() {}

public:
	virtual void HandleCallback(void* pCallbackData, float TrackPosition) {}
};

// #define _WITH_ANIMATION_SRT

class AnimationSet
{
public:
	AnimationSet();
	~AnimationSet();

public:
	char m_strAnimationName[64] = {'\0'};

	float m_Length = 0.0f;
	int m_nFramesPerSecond = 0;

	int m_nKeyFrames = 0;
	float m_KeyFrameTimes = NULL;
	std::vector<XMFLOAT4X4> m_xmf4x4KeyFrameTransforms;

#ifdef _WITH_ANIMATION_SRT
	int m_nKeyFrameScales = 0;
	float 


#endif // _WITH_ANIMATION_SRT

public:
	XMFLOAT4X4 GetSRT(int nBone, float Position);
};

class AnimationSets
{
public:
	AnimationSets();
	~AnimationSets();

public:
	int m_nAnimationSets = 0;
	std::vector<std::shared_ptr<AnimationSet>> m_pAnimationSets;

	int m_nAnimatedBoneFrames = 0;
	std::vector<std::shared_ptr<Object>> m_pAnimatedBoneFrameCaches;
};

class AnimationTrack
{
public:
	AnimationTrack();
	~AnimationTrack();

public:
	int m_nCallbackKeys = 0;
	std::vector<CALLBACKKEY> m_CallbackKeys;

	std::shared_ptr<AnimationCallbackHandler> m_pAnimationCallbackHandler = nullptr;

public:
	float UpdatePosition(float TrackPosition, float TrackElapsedTime, float AnimationLength);
	void HandleCallback();

public:
	bool m_bEnable = true;
	float m_Speed = 1.0f;
	float m_Position = -ANIMATION_CALLBACK_EPSILON;
	float m_Weight = 1.0f;

	int m_nAnimationSet = 0;

	int m_Type = ANIMATION_TYPE_LOOP;		//Once, Loop, PingPong

public:
	void SetEnable(bool bEnable) { m_bEnable = bEnable;	}
	void SetSpeed(float Speed) { m_Speed = Speed; }
	void SetPosition(float Position) { m_Position = Position; }
	void SetWeight(float Weight) { m_Weight = Weight; }

	void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }
	void SetType(int Type) { m_Type = Type; }

	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float KeyTime, void* pData);
	void SetAnimationCallbackHandler(std::shared_ptr<AnimationCallbackHandler> pCallbackHandler);

};

class ModelDataInfo
{
public:
	ModelDataInfo();
	~ModelDataInfo();

	std::shared_ptr<Object> m_pRootObject = nullptr;

	int m_nSkinMeshes = 0;
	std::vector<std::shared_ptr<SkinnedMesh>> m_pSkinnedMeshes;

	std::shared_ptr<AnimationSets> m_pAnimationSets = nullptr;

public:
	void PrepareSkinning();

};

class AnimationController
{
public:
	AnimationController();
	~AnimationController();

public:
	float m_Time = 0.0f;

	int m_nAnimationTracks = 0;
	std::vector<AnimationTrack> m_AnimationTracks;

public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void AdvanceTime(float ElapsedTime, Object* pRootGameObject);

};


