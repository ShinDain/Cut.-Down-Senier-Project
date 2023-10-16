#include "../Header/Cinematic.h"
#include "../Header/Character.h"
#include "../../DirectXRendering/Header/Scene.h"

Cinematic::Cinematic()
{
	AddSoundKeyFrame(0.0f, "", 0.0f);
}

Cinematic::~Cinematic()
{
}

void Cinematic::Play()
{
	m_bPlay = true;
}

void Cinematic::Stop()
{
	m_bPlay = false;
}

void Cinematic::Update(float elapsedTime)
{
	if (m_bPlay)
	{
		m_ElapsedTime += elapsedTime;
		
		if (!m_CameraTrack.TrackEnd)
		{
			// 시네마틱 카메라 업데이트
			if (m_ElapsedTime > m_CameraTrack.KeyFrameTimes[m_CameraTrack.nCurIdx])
			{
				m_CameraTrack.nCurIdx += 1;
				if (m_CameraTrack.nKeyFrameCnt == m_CameraTrack.nCurIdx)
					m_CameraTrack.TrackEnd = true;
			}

			UINT cameraCurIdx = m_CameraTrack.nCurIdx;
			MoveCameraKeyFrameData(cameraCurIdx);
		}

		if (!m_SoundTrack.TrackEnd)
		{
			// 사운드 트랙 업데이트
			if (m_ElapsedTime > m_SoundTrack.KeyFrameTimes[m_SoundTrack.nCurIdx])
			{
				UINT soundCurIdx = m_SoundTrack.nCurIdx;
				if (soundCurIdx < m_SoundTrack.nKeyFrameCnt && soundCurIdx != 0)
				{
					if (m_SoundTrack.SoundKeyFrames[soundCurIdx].volume > 0)
					{
						float volume = m_SoundTrack.SoundKeyFrames[soundCurIdx].volume;
						const char* pstrFilePath = m_SoundTrack.SoundKeyFrames[soundCurIdx].AudioFilePath;
						Scene::EmitSound(pstrFilePath, false, 1.0f, volume);
					}
				}

				m_SoundTrack.nCurIdx += 1;
				if (m_SoundTrack.nKeyFrameCnt == m_SoundTrack.nCurIdx)
					m_SoundTrack.TrackEnd = true;
			}
		}

		// 참조된 오브젝트를 순회하며 업데이트
		for (int i = 0; i < m_vCinematicTracks.size(); ++i)
		{
			if (m_vCinematicTracks[i].TrackEnd)
				continue;

			if (m_ElapsedTime > m_vCinematicTracks[i].KeyFrameTimes[m_vCinematicTracks[i].nCurIdx])
			{
				auto func = m_vCinematicTracks[i].KeyFrames[m_vCinematicTracks[i].nCurIdx].pFunction;
				if(func)
					(m_vCinematicTracks[i].pObject.get()->*func)();

				m_vCinematicTracks[i].nCurIdx += 1;
				if (m_vCinematicTracks[i].nKeyFrameCnt == m_vCinematicTracks[i].nCurIdx)
					m_vCinematicTracks[i].TrackEnd = true;
			}

			UINT nCurIdx = m_vCinematicTracks[i].nCurIdx;
			MoveKeyFrameData(i, nCurIdx);
		}
	}

	if (m_ElapsedTime > m_EndTime)
	{
		m_bPlay = false;
		m_bCinematicEnd = true;
		return;
	}
}

void Cinematic::Destroy()
{
	std::vector<CinematicTrack> m_vCinematicTracks;
	CinematicCameraTrack m_CameraTrack;

	for (int i = 0; i < m_vCinematicTracks.size(); ++i)
	{
		m_vCinematicTracks[i].pObject.reset();
	}

	m_CameraTrack.pCamera.reset();
}

void Cinematic::AddTrack(std::shared_ptr<Object> pObject, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale)
{
	CinematicTrack newTrack;
	newTrack.pObject = pObject;

	m_vCinematicTracks.emplace_back(newTrack);

	KeyFrame initKey;
	initKey.pFunction = nullptr;
	initKey.xmf3Position = xmf3Position;
	initKey.xmf3Rotation = xmf3Rotation;
	initKey.xmf3Scale = xmf3Scale;

	int idx = m_vCinematicTracks.size() - 1;
	m_vCinematicTracks[idx].KeyFrameTimes.emplace_back(0.0);
	m_vCinematicTracks[idx].KeyFrames.emplace_back(initKey);
	m_vCinematicTracks[idx].nKeyFrameCnt += 1;
}

void Cinematic::AddKeyFrame(UINT nTrackIdx, float time, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale, void(Object::* pFunction)())
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

void Cinematic::AddCamera(std::shared_ptr<Camera> pCamera, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation)
{
	m_CameraTrack.pCamera = pCamera;

	// 오브젝트 추가시 자동으로 0.0f 키프레임 초기화
	KeyFrame newKeyFrame;
	newKeyFrame.xmf3Position = xmf3Position;
	newKeyFrame.xmf3Rotation = xmf3Rotation;

	newKeyFrame.pFunction = nullptr;

	m_CameraTrack.KeyFrames.push_back(newKeyFrame);
	m_CameraTrack.KeyFrameTimes.push_back(0.0f);
	m_CameraTrack.nKeyFrameCnt += 1;
}

void Cinematic::AddCameraKeyFrame(float time, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation)
{
	KeyFrame newKeyFrame;
	newKeyFrame.xmf3Position = xmf3Position;
	newKeyFrame.xmf3Rotation = xmf3Rotation;
	newKeyFrame.pFunction = nullptr;

	m_CameraTrack.KeyFrames.push_back(newKeyFrame);
	m_CameraTrack.KeyFrameTimes.push_back(time);
	m_CameraTrack.nKeyFrameCnt += 1;

	if (m_EndTime < time)
		m_EndTime = time;
}

void Cinematic::AddSoundKeyFrame(float time, const char* pstrFilePath, float volume)
{
	SoundKeyFrame newKeyFrame;
	newKeyFrame.volume = volume;
	newKeyFrame.AudioFilePath = pstrFilePath;
	
	m_SoundTrack.SoundKeyFrames.push_back(newKeyFrame);
	m_SoundTrack.KeyFrameTimes.push_back(time);
	m_SoundTrack.nKeyFrameCnt += 1;

	if (m_EndTime < time)
		m_EndTime = time;
}

// 현재 -> 키프레임
void Cinematic::MoveCameraKeyFrameData(UINT nCurIdx)
{
	if (nCurIdx < m_CameraTrack.nKeyFrameCnt && nCurIdx != 0)
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
		XMFLOAT4 xmf4NewOrientation;
		XMVECTOR newOrientation;
		XMVECTOR prevOrientation = XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(cameraPrevKeyFrame.xmf3Rotation.x),
			XMConvertToRadians(cameraPrevKeyFrame.xmf3Rotation.y),
			XMConvertToRadians(cameraPrevKeyFrame.xmf3Rotation.z));
		XMVECTOR nextOrientation = XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(cameraNextKeyFrame.xmf3Rotation.x),
			XMConvertToRadians(cameraNextKeyFrame.xmf3Rotation.y),
			XMConvertToRadians(cameraNextKeyFrame.xmf3Rotation.z));
		newOrientation = XMQuaternionSlerp(prevOrientation, nextOrientation, weight);
		XMStoreFloat4(&xmf4NewOrientation, newOrientation);

		Camera* pCamera = m_CameraTrack.pCamera.get();
		pCamera->SetPosition(xmf3NewPosition);
		pCamera->SetOrientation(xmf4NewOrientation);
	}
}

void Cinematic::MoveKeyFrameData(UINT nTrackIdx, UINT nCurIdx)
{
	if (nTrackIdx > m_vCinematicTracks.size())
		return;

	if (nCurIdx < m_vCinematicTracks[nTrackIdx].nKeyFrameCnt && nCurIdx != 0)
	{
		Object* pObject = m_vCinematicTracks[nTrackIdx].pObject.get();
		bool IsCharacter = pObject->GetObjectType() == Object_Monster || pObject->GetObjectType() == Object_Player ? true : false;

		float prevTime = m_vCinematicTracks[nTrackIdx].KeyFrameTimes[nCurIdx - 1];
		float nextTime = m_vCinematicTracks[nTrackIdx].KeyFrameTimes[nCurIdx];
		KeyFrame prevKeyFrame = m_vCinematicTracks[nTrackIdx].KeyFrames[nCurIdx - 1];
		KeyFrame nextKeyFrame = m_vCinematicTracks[nTrackIdx].KeyFrames[nCurIdx];

		float weight = (m_ElapsedTime - prevTime) / (nextTime - prevTime);

		
		if(IsCharacter)
		{
			// Position 보간
			XMFLOAT3 xmf3NewPosition;
			XMVECTOR newPosition;
			XMVECTOR prevPosition = XMLoadFloat3(&prevKeyFrame.xmf3Position);
			XMVECTOR nextPosition = XMLoadFloat3(&nextKeyFrame.xmf3Position);
			newPosition = XMVectorLerp(prevPosition, nextPosition, weight);
			XMStoreFloat3(&xmf3NewPosition, newPosition);
			pObject->GetBody()->SetPosition(xmf3NewPosition);
			pObject->SetPosition(xmf3NewPosition);

			XMFLOAT3 xmf3NewRotation;
			XMVECTOR newRotation;
			XMVECTOR prevRotation = XMLoadFloat3(&prevKeyFrame.xmf3Rotation);
			XMVECTOR nextRotation = XMLoadFloat3(&nextKeyFrame.xmf3Rotation);
			newRotation = XMVectorLerp(prevRotation, nextRotation, weight);
			XMStoreFloat3(&xmf3NewRotation, newRotation);
			// Character는 Yaw 회전만 적용된다.
			pObject->GetBody()->SetRotate(xmf3NewRotation);
		}
		else
		{
			// Position 보간
			XMFLOAT3 xmf3NewPosition;
			XMVECTOR newPosition;
			XMVECTOR prevPosition = XMLoadFloat3(&prevKeyFrame.xmf3Position);
			XMVECTOR nextPosition = XMLoadFloat3(&nextKeyFrame.xmf3Position);
			newPosition = XMVectorLerp(prevPosition, nextPosition, weight);
			XMStoreFloat3(&xmf3NewPosition, newPosition);
			pObject->GetBody()->SetPosition(xmf3NewPosition);
			pObject->SetPosition(xmf3NewPosition);

			XMFLOAT4 xmf4NewOrientation;
			XMVECTOR newOrientation;
			XMVECTOR prevOrientation = XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(prevKeyFrame.xmf3Rotation.x),
				XMConvertToRadians(prevKeyFrame.xmf3Rotation.y),
				XMConvertToRadians(prevKeyFrame.xmf3Rotation.z));
			XMVECTOR nextOrientation = XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(nextKeyFrame.xmf3Rotation.x),
				XMConvertToRadians(nextKeyFrame.xmf3Rotation.y),
				XMConvertToRadians(nextKeyFrame.xmf3Rotation.z));
			newOrientation = XMQuaternionSlerp(prevOrientation, nextOrientation, weight);
			newOrientation = XMQuaternionSlerp(prevOrientation, nextOrientation, weight);
			XMStoreFloat4(&xmf4NewOrientation, newOrientation);
			pObject->GetBody()->SetOrientation(xmf4NewOrientation);
			pObject->SetOrientation(xmf4NewOrientation);
		}

		// Scale 보간
		XMFLOAT3 xmf3NewScale;
		XMVECTOR newScale;
		XMVECTOR prevScale = XMLoadFloat3(&prevKeyFrame.xmf3Scale);
		XMVECTOR nextScale = XMLoadFloat3(&nextKeyFrame.xmf3Scale);
		newScale = XMVectorLerp(prevScale, nextScale, weight);
		XMStoreFloat3(&xmf3NewScale, newScale);
		pObject->GetBody()->SetScale(xmf3NewScale);
		pObject->SetScale(xmf3NewScale);
	}
}

