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
	std::vector<float> m_vKeyFrameTimes;
	std::vector<std::vector<XMFLOAT4X4>> m_vvxmf4x4KeyFrameTransforms;		// 키프레임 개수만큼의 변환을 각 뼈마다

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
	std::vector<std::shared_ptr<AnimationSet>> m_vpAnimationSets;

	int m_nAnimatedBoneFrames = 0;
	std::vector<std::shared_ptr<Object>> m_vpAnimatedBoneFrameCaches;
};

class AnimationTrack
{
public:
	AnimationTrack();
	~AnimationTrack();

public:
	int m_nCallbackKeys = 0;
	std::vector<CALLBACKKEY> m_vCallbackKeys;

	std::shared_ptr<AnimationCallbackHandler> m_pAnimationCallbackHandler = nullptr;

public:
	float UpdatePosition(float TrackPosition, float ElapsedTime, float AnimationLength);
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

	int m_nSkinnedMeshes = 0;
	std::vector<std::shared_ptr<SkinnedMesh>> m_vpSkinnedMeshes;

	std::vector<std::shared_ptr<AnimationSets>> m_vpAnimationSets;

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
	std::vector<AnimationTrack> m_vAnimationTracks;

	std::vector<AnimationSets> m_vAnimationSets;

	int m_nSkinnedMeshes = 0;
	std::vector<std::shared_ptr<SkinnedMesh>> m_vpSkinnedMeshes;

	//--------------------

	// skinnedMesh 개수 * Bone 개수 * xmfloat4x4 만큼의 버퍼
	std::vector<std::shared_ptr<UploadBuffer<XMFLOAT4X4>>> m_SkinningBoneTransformCBs;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_vSkinningBoneTransforms;
	std::vector<std::vector<XMFLOAT4X4>> m_vvxmf4x4MappedSkinningBoneTransfroms;

	//---------------------

public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void AdvanceTime(float ElapsedTime, Object* pRootGameObject);

public:
	bool	m_bRootMotion = false;
	Object* m_pModelRootObject = NULL;

	Object* m_pRootMotionObject = NULL;
	XMFLOAT3 m_xmf3FirstRootMotionPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	void SetRootMotion(bool bRootMotion) { m_bRootMotion = bRootMotion; }

	// 미구현
	// virtual void OnRootMotion(Object* pRootGameObject) { }
	// virtual void OnAnimationIK(Object* pRootGameObject) { }

public:
	// Set Track
	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
	{
		if (nAnimationTrack <= m_vAnimationTracks.size()) m_vAnimationTracks[nAnimationTrack].m_nAnimationSet = nAnimationSet;
	}
	void SetTrackEnable(int nAnimationTrack, bool bEnable)
	{
		if (nAnimationTrack <= m_vAnimationTracks.size()) m_vAnimationTracks[nAnimationTrack].SetEnable(bEnable);
	}
	void SetTrackPosition(int nAnimationTrack, float Position)
	{
		if (nAnimationTrack <= m_vAnimationTracks.size()) m_vAnimationTracks[nAnimationTrack].SetPosition(Position);
	}
	void SetTrackSpeed(int nAnimationTrack, float Speed)
	{
		if (nAnimationTrack <= m_vAnimationTracks.size()) m_vAnimationTracks[nAnimationTrack].SetSpeed(Speed);
	}
	void SetTrackWeight(int nAnimationTrack, float Weight)
	{
		if (nAnimationTrack <= m_vAnimationTracks.size()) m_vAnimationTracks[nAnimationTrack].SetWeight(Weight);
	}

	// Set Callback
	void SetCallbackKeys(int nAnimationTrack, int nCallbackKeys)
	{
		if (nAnimationTrack <= m_vAnimationTracks.size()) m_vAnimationTracks[nAnimationTrack].SetCallbackKeys(nCallbackKeys);
	}
	void SetCallbackKey(int nAnimationTrack, int nKeyidx, float Time, void* pData)
	{
		if (nAnimationTrack <= m_vAnimationTracks.size()) m_vAnimationTracks[nAnimationTrack].SetCallbackKey(nKeyidx, Time, pData);
	}
	void SetAnimationCallbackHandler(int nAnimationTrack, std::shared_ptr<AnimationCallbackHandler> pCallbackHandler)
	{
		if (nAnimationTrack <= m_vAnimationTracks.size()) m_vAnimationTracks[nAnimationTrack].SetAnimationCallbackHandler(pCallbackHandler);
	}
};


