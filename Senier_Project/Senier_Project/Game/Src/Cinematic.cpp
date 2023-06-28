#include "../Header/Cinematic.h"

Cinematic::Cinematic()
{
}

Cinematic::~Cinematic()
{
}

void Cinematic::Play()
{
	m_bPlay = true;
}

void Cinematic::Update(float elapsedTime)
{
	if (m_bPlay)
	{
		m_ElapsedTime += elapsedTime;
		if (m_ElapsedTime > m_EndTime)
		{
			m_bPlay = false;
			m_bCinematicEnd = true;
			return;
		}
		
		// 시네마틱 카메라 업데이트
		if (m_ElapsedTime > m_CameraTrack.KeyFrameTimes[m_CameraTrack.nCurIdx])
			m_CameraTrack.nCurIdx += 1;

		UINT cameraCurIdx = m_CameraTrack.nCurIdx;
		GetCameraKeyFrameData(cameraCurIdx);

		//// 참조된 오브젝트를 순회하며 업데이트
		//for (int i = 0; i < m_vCinematicTracks.size(); ++i)
		//{
		//
		//
		//
		//}
	}
}

void Cinematic::AddTrack(std::shared_ptr<Object> pObject)
{
	CinematicTrack newTrack;
	newTrack.pObject = pObject;

	m_vCinematicTracks.emplace_back(newTrack);

	KeyFrame initKey;
	initKey.pFunction = nullptr;
	initKey.xmf3Position = pObject->GetPosition();
	initKey.xmf3Rotation = pObject->GetRotation();
	initKey.xmf3Scale = pObject->GetScale();
	initKey.xmf4Orientation = pObject->GetOrientation();

	int idx = m_vCinematicTracks.size() - 1;
	m_vCinematicTracks[idx].KeyFrameTimes.emplace_back(0.0);
	m_vCinematicTracks[idx].KeyFrames.emplace_back(initKey);
	m_vCinematicTracks[idx].nKeyFrameCnt += 1;
}

void Cinematic::AddKeyFrame(UINT nTrackIdx, float time, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale, void(*pFunction)())
{
	// 잘못된 idx를 참조할 경우
	if (nTrackIdx > m_vCinematicTracks.size())
		return;

	KeyFrame newKeyFrame;
	newKeyFrame.xmf3Position = xmf3Position;
	newKeyFrame.xmf3Rotation = xmf3Rotation;
	newKeyFrame.xmf3Scale = xmf3Scale;
	newKeyFrame.pFunction = pFunction;

	m_vCinematicTracks[nTrackIdx].KeyFrames.push_back(newKeyFrame);
	m_vCinematicTracks[nTrackIdx].KeyFrameTimes.push_back(time);
	m_vCinematicTracks[nTrackIdx].nKeyFrameCnt += 1;

	if (m_EndTime < time)
		m_EndTime = time;

}

void Cinematic::AddCameraKeyFrame(float time, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale)
{
	KeyFrame newKeyFrame;
	newKeyFrame.xmf3Position = xmf3Position;
	newKeyFrame.xmf3Rotation = xmf3Rotation;
	newKeyFrame.xmf3Scale = xmf3Scale;
	newKeyFrame.pFunction = nullptr;

	m_CameraTrack.KeyFrames.push_back(newKeyFrame);
	m_CameraTrack.KeyFrameTimes.push_back(time);
	m_CameraTrack.nKeyFrameCnt += 1;

	if (m_EndTime < time)
		m_EndTime = time;
}

// 현재 -> 키프레임
void Cinematic::GetCameraKeyFrameData(UINT nCurIdx)
{
	if (nCurIdx < m_CameraTrack.nKeyFrameCnt)
	{
		float cameraPrevTime = m_CameraTrack.KeyFrameTimes[nCurIdx - 1];
		float cameraNextTime = m_CameraTrack.KeyFrameTimes[nCurIdx];
		KeyFrame cameraPrevKeyFrame = m_CameraTrack.KeyFrames[nCurIdx - 1];
		KeyFrame cameraNextKeyFrame = m_CameraTrack.KeyFrames[nCurIdx];

		float weight = (m_ElapsedTime - cameraPrevTime) / (cameraNextTime - cameraPrevTime);

		// Position 보간
		XMFLOAT3 xmf3NewPosition;
		XMVECTOR newPosition;
		XMVECTOR prevPosition = XMLoadFloat3(&cameraPrevKeyFrame.xmf3Position);
		XMVECTOR nextPosition = XMLoadFloat3(&cameraNextKeyFrame.xmf3Position);
		newPosition = XMVectorLerp(prevPosition, nextPosition, weight);
		XMStoreFloat3(&xmf3NewPosition, newPosition);

		// Rotation 보간
		XMFLOAT4 xmf4Orientation;
		XMVECTOR newOrientation;
		XMVECTOR prevRotation = XMLoadFloat3(&cameraPrevKeyFrame.xmf3Rotation);
		XMVECTOR prevOrientation = XMQuaternionRotationRollPitchYawFromVector(prevRotation);
		XMVECTOR nextRotation = XMLoadFloat3(&cameraNextKeyFrame.xmf3Rotation);
		XMVECTOR nextOrientation = XMQuaternionRotationRollPitchYawFromVector(nextRotation);
		newOrientation = XMQuaternionSlerp(prevOrientation, nextOrientation, weight);
		XMStoreFloat4(&xmf4Orientation, newOrientation);

		Camera* pCamera = m_CameraTrack.pCamera.get();
		pCamera->SetPosition(xmf3NewPosition);
		pCamera->SetOrientation(xmf4Orientation);
	}
}

void Cinematic::GetKeyFrameData(UINT nCurIdx)
{
}

void Cinematic::SetCamera(std::shared_ptr<Camera> pCamera)
{
	m_CameraTrack.pCamera = pCamera;

	// 오브젝트 추가시 자동으로 0.0f 키프레임 초기화
	KeyFrame newKeyFrame;
	newKeyFrame.xmf3Position = pCamera->GetPosition3f();
	newKeyFrame.xmf3Rotation = XMFLOAT3(0, 0, 0);
	newKeyFrame.xmf4Orientation = XMFLOAT4(0, 0, 0, 1);
	
	newKeyFrame.pFunction = nullptr;

	m_CameraTrack.KeyFrames.push_back(newKeyFrame);
	m_CameraTrack.KeyFrameTimes.push_back(0.0f);
	m_CameraTrack.nKeyFrameCnt += 1;
}
