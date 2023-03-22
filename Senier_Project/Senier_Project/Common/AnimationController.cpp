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
	//	SRT�� ���� ���� ��� ��
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
	// Animation�� �ε�Ǹ� �ʱ�ȭ��
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

AnimationController::AnimationController()
{
	m_SkinningBoneTransformCBs.resize(m_nSkinnedMeshes);

	// ���� �ڵ忡���� ������ �� �ִ� �ִ� ũ���� ���۸� ����
	// ���� ���� ��ŭ�� ���۸� �����ٸ�??
	//for (int i = 0; i < m_nSkinnedMeshes; ++i)
		//m_SkinningBoneTransformCBs[i] = std::make_unique<UploadBuffer<XMFLOAT4X4>>(pd3dDevice, m_nskinn;
}

AnimationController::~AnimationController()
{
}

void AnimationController::AdvanceTime(float ElapsedTime, Object* pRootGameObject)
{
}

void AnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; ++i)
	{
		m_vpSkinnedMeshes[i]->SetSkinningBoneTransformCB(m_SkinningBoneTransformCBs[i]);
	}
}
