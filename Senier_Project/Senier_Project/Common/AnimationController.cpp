#include "AnimationController.h"
#include "Mesh.h"
#include "Object.h"

AnimationSet::AnimationSet()
{
}

AnimationSet::~AnimationSet()
{
}

XMFLOAT4X4 AnimationSet::GetSRT(int nBone, float Position)
{
	XMFLOAT4X4 xmf4x4Transform = MathHelper::identity4x4();

#ifdef _WITH_ANIMATION_SRT
	//	
	//	SRT를 따로 따로 계산 필
	//
#else
	for (int i = 0; i < (m_nKeyFrames - 1); ++i)
	{
		if ((m_vKeyFrameTimes[i] <= Position) && (Position < m_vKeyFrameTimes[i + 1]))
		{
			float t = (Position - m_vKeyFrameTimes[i]) / (m_vKeyFrameTimes[i + 1] - m_vKeyFrameTimes[i]);
			xmf4x4Transform = MathHelper::XMFloat4x4Interpolate(m_vvxmf4x4KeyFrameTransforms[i + 1][nBone], m_vvxmf4x4KeyFrameTransforms[i][nBone], t);
			break;
		}
	}
	if (Position >= m_vKeyFrameTimes[m_nKeyFrames - 1]) xmf4x4Transform = m_vvxmf4x4KeyFrameTransforms[m_nKeyFrames][nBone];

#endif

	return 	xmf4x4Transform;
}

////////////////////////////////////////////////////////////////

AnimationSets::AnimationSets()
{
	// Animation이 로드되며 초기화됨
}

AnimationSets::~AnimationSets()
{
}

////////////////////////////////////////////////////////////////

AnimationTrack::AnimationTrack()
{
}

AnimationTrack::~AnimationTrack()
{
}

void AnimationTrack::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_vCallbackKeys.resize(nCallbackKeys);
}

void AnimationTrack::SetCallbackKey(int nKeyIndex, float KeyTime, void* pData)
{
	m_vCallbackKeys[nKeyIndex].m_Time = KeyTime;
	m_vCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void AnimationTrack::SetAnimationCallbackHandler(std::shared_ptr<AnimationCallbackHandler> pCallbackHandler)
{
	m_pAnimationCallbackHandler = pCallbackHandler;
}

float AnimationTrack::UpdatePosition(float TrackPosition, float ElapsedTime, float AnimationLength)
{
	float fTrackElapsedTime = ElapsedTime * m_Speed;

	switch (m_Type)
	{
	case ANIMATION_TYPE_LOOP:
		if (m_Position < 0.0f) m_Position = 0.0f;
		else
		{
			m_Position = TrackPosition + fTrackElapsedTime;
			if (m_Position > AnimationLength)
			{
				m_Position = -ANIMATION_CALLBACK_EPSILON;
				return (AnimationLength);
			}
		}

		break;

	case ANIMATION_TYPE_ONCE:
		m_Position = TrackPosition + fTrackElapsedTime;
		if (m_Position > AnimationLength) m_Position = AnimationLength;
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;

	}

	return m_Position;
}

void AnimationTrack::HandleCallback()
{
	if (m_pAnimationCallbackHandler)
	{
		for (int i = 0; i < m_nCallbackKeys; ++i)
		{
			if (MathHelper::IsEqual(m_vCallbackKeys[i].m_Time, m_Position, ANIMATION_CALLBACK_EPSILON))
			{
				if (m_vCallbackKeys[i].m_pCallbackData)
					m_pAnimationCallbackHandler->HandleCallback(m_vCallbackKeys[i].m_pCallbackData, m_Position);

				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////

ModelDataInfo::ModelDataInfo()
{
}

ModelDataInfo::~ModelDataInfo()
{
}

void ModelDataInfo::PrepareSkinning()
{
	int nSkinnedMesh = 0;
	m_vpSkinnedMeshes.resize(m_nSkinnedMeshes);
	m_pRootObject->FindAndSetSkinnedMesh(m_vpSkinnedMeshes);

	for (int i = 0; i < m_nSkinnedMeshes; ++i)
		m_vpSkinnedMeshes[i]->PrepareSkinning(m_pRootObject.get());
}

////////////////////////////////////////////////////////////////

AnimationController::AnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, ModelDataInfo* pModel)
{
	m_nAnimationTracks = nAnimationTracks;
	m_vpAnimationTracks.resize(nAnimationTracks);

	m_pAnimationSets = pModel->m_pAnimationSets;

	m_pModelRootObject = pModel->m_pRootObject;

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;

	m_vpSkinnedMeshes.resize(m_nSkinnedMeshes);
	for (int i = 0; i < m_nSkinnedMeshes; ++i) m_vpSkinnedMeshes[i] = pModel->m_vpSkinnedMeshes[i];

	m_SkinningBoneTransformCBs.resize(m_nSkinnedMeshes);

	// 원본 코드에서는 가정할 수 있는 최대 크기의 버퍼를 생성
	// 뼈의 개수 만큼만 버퍼를 가진다면

	for (int i = 0; i < m_nSkinnedMeshes; ++i)
	{
		int nBoneCnt = m_vpSkinnedMeshes[i]->GetSkinningBones();
		m_SkinningBoneTransformCBs[i] = std::make_unique<UploadBuffer<XMFLOAT4X4>>(pd3dDevice, nBoneCnt, true);
	}
}

AnimationController::~AnimationController()
{
}

void AnimationController::AdvanceTime(float ElapsedTime, Object* pRootGameObject)
{
	m_Time += ElapsedTime;
	if (m_vpAnimationTracks.size() > 0)
	{
		for (int i = 0; i < m_pAnimationSets->m_nAnimatedBoneFrames; ++i)
			m_pAnimationSets->m_vpAnimatedBoneFrameCaches[i]->SetLocalTransform(MathHelper::Zero4x4());

		for (int k = 0; k < m_nAnimationTracks; ++k)
		{
			if (m_vpAnimationTracks[k]->m_bEnable)
			{
				AnimationSet* pAnimationSet = m_pAnimationSets->m_vpAnimationSets[m_vpAnimationTracks[k]->m_nAnimationSet].get();
				float fPosition = m_vpAnimationTracks[k]->UpdatePosition(m_vpAnimationTracks[k]->m_Position, ElapsedTime, pAnimationSet->m_Length);

				for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; ++j)
				{
					XMFLOAT4X4 xmf4x4Transform = m_pAnimationSets->m_vpAnimatedBoneFrameCaches[j]->GetLocalTransform();
					XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j, fPosition);
					XMStoreFloat4x4(&xmf4x4Transform, XMLoadFloat4x4(&xmf4x4Transform) + XMLoadFloat4x4(&xmf4x4TrackTransform) * m_vpAnimationTracks[k]->m_Weight);


					m_pAnimationSets->m_vpAnimatedBoneFrameCaches[j]->SetLocalTransform(xmf4x4Transform);
				}
				m_vpAnimationTracks[k]->HandleCallback();
			}
		}


		OnRootMotion(pRootGameObject);
	}
}

void AnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; ++i)
	{
		m_vpSkinnedMeshes[i]->SetSkinningBoneTransformCB(m_SkinningBoneTransformCBs[i]);
	}
}