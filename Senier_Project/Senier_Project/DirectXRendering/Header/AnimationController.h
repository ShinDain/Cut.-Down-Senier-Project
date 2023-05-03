#pragma once

#include "Global.h"
#include "../../Common/Header/D3DUtil.h"

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

	int m_nAnimationSet = 0;			// n번 애니메이션, 개수 아님

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

	std::shared_ptr<AnimationSets> m_pAnimationSets;

public:
	void PrepareSkinning();

};

class AnimationController
{
public:
	AnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, std::shared_ptr<ModelDataInfo> pModel);
	~AnimationController();

	void ChangeBoneTransformCB(ID3D12GraphicsCommandList* pd3dCommandList);
	void AdvanceTime(float ElapsedTime, Object* pRootGameObject);

public:
	float m_Time = 0.0f;

	int m_nAnimationTracks = 0;
	std::vector<std::shared_ptr<AnimationTrack>> m_vpAnimationTracks;

	std::shared_ptr<AnimationSets> m_pAnimationSets;

	int m_nSkinnedMeshes = 0;
	std::vector<std::shared_ptr<SkinnedMesh>> m_vpSkinnedMeshes;

	//--------------------

	UINT m_nxmf4x4CBByteSize = 0;

	std::vector<std::shared_ptr<UploadBuffer<SkinningBoneTransformConstant>>> m_vSkinningBoneTransformCBs;
	//---------------------

public:
	bool	m_bRootMotion = false;
	std::shared_ptr<Object> m_pModelRootObject = NULL;

	std::shared_ptr<Object> m_pRootMotionObject = NULL;
	XMFLOAT3 m_xmf3FirstRootMotionPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	void SetRootMotion(bool bRootMotion) { m_bRootMotion = bRootMotion; }

	virtual void OnRootMotion(Object* pRootGameObject) { }

	// 미구현
	// virtual void OnAnimationIK(Object* pRootGameObject) { }

public:
	// Set Track
	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
	{
		if (nAnimationTrack < m_vpAnimationTracks.size()) m_vpAnimationTracks[nAnimationTrack]->m_nAnimationSet = nAnimationSet;
	}
	void SetTrackEnable(int nAnimationTrack, bool bEnable)
	{
		if (nAnimationTrack < m_vpAnimationTracks.size()) m_vpAnimationTracks[nAnimationTrack]->SetEnable(bEnable);
	}
	void SetTrackPosition(int nAnimationTrack, float Position)
	{
		if (nAnimationTrack < m_vpAnimationTracks.size()) m_vpAnimationTracks[nAnimationTrack]->SetPosition(Position);
	}
	void SetTrackSpeed(int nAnimationTrack, float Speed)
	{
		if (nAnimationTrack < m_vpAnimationTracks.size()) m_vpAnimationTracks[nAnimationTrack]->SetSpeed(Speed);
	}
	void SetTrackWeight(int nAnimationTrack, float Weight)
	{
		if (nAnimationTrack < m_vpAnimationTracks.size()) m_vpAnimationTracks[nAnimationTrack]->SetWeight(Weight);
	}

	// Set Callback
	void SetCallbackKeys(int nAnimationTrack, int nCallbackKeys)
	{
		if (nAnimationTrack <= m_vpAnimationTracks.size()) m_vpAnimationTracks[nAnimationTrack]->SetCallbackKeys(nCallbackKeys);
	}
	void SetCallbackKey(int nAnimationTrack, int nKeyidx, float Time, void* pData)
	{
		if (nAnimationTrack <= m_vpAnimationTracks.size()) m_vpAnimationTracks[nAnimationTrack]->SetCallbackKey(nKeyidx, Time, pData);
	}
	void SetAnimationCallbackHandler(int nAnimationTrack, std::shared_ptr<AnimationCallbackHandler> pCallbackHandler)
	{
		if (nAnimationTrack <= m_vpAnimationTracks.size()) m_vpAnimationTracks[nAnimationTrack]->SetAnimationCallbackHandler(pCallbackHandler);
	}
};


