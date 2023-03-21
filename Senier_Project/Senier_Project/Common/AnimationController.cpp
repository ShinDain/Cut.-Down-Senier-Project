#include "AnimationController.h"
#include "Mesh.h"

AnimationSet::AnimationSet()
{
}

AnimationSet::~AnimationSet()
{
}

XMFLOAT4X4 AnimationSet::GetSRT(int nBone, float Position)
{
	return XMFLOAT4X4();
}

////////////////////////////////////////////////////////////////

AnimationSets::AnimationSets()
{
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

float AnimationTrack::UpdatePosition(float TrackPosition, float TrackElapesd, float AnimationLength)
{
	return 0.0f;
}

void AnimationTrack::HandleCallback()
{
}

void AnimationTrack::SetCallbackKeys(int nCallbackKeys)
{
}

void AnimationTrack::SetCallbackKey(int, float KeyTime, void* pData)
{
}

void AnimationTrack::SetAnimationCallbackHandler(std::shared_ptr<AnimationCallbackHandler> p)
{
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

}

////////////////////////////////////////////////////////////////

AnimationController::AnimationController()
{
	m_SkinningBoneTransformCBs.resize(m_nSkinnedMeshes);

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
