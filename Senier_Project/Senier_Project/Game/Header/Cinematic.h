#ifndef CINEMATIC_H

#define CINEMATIC_H

#include <vector>
#include <map>
#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/MathHelper.h"
#include "../../DirectXRendering/Header/Camera.h"
#include "../../DirectXRendering/Header/Object.h"

#define KEYFRAME_FUNCTION_EPSILON	0.00165f

// a 키프레임에 p오브젝트가 특정 이동/회전으로 조정된다.
// 특정 애니메이션을 재생시키려면,
// 특정 함수를 재생시키는 것이 유리
struct KeyFrame // map에 시간과 함께 배치
{
	XMFLOAT3 xmf3Position;
	XMFLOAT3 xmf3Rotation;
	XMFLOAT3 xmf3Scale;

	void(Object::*pFunction)();
};

struct SoundKeyFrame // map에 시간과 함께 배치
{
	float volume = 0.5f;
	const char* AudioFilePath;;
};


struct CinematicTrack
{
	std::shared_ptr<Object> pObject;
	bool TrackEnd = false;

	UINT nCurIdx = 0;
	UINT nKeyFrameCnt = 0;

	std::vector<float> KeyFrameTimes;
	std::vector<KeyFrame> KeyFrames;
};

struct CinematicCameraTrack
{
	std::shared_ptr<Camera> pCamera;
	bool TrackEnd = false;

	UINT nCurIdx = 0;
	UINT nKeyFrameCnt = 0;

	std::vector<float> KeyFrameTimes;
	std::vector<KeyFrame> KeyFrames;
};

struct CinematicSoundTrack
{
	bool TrackEnd = false;

	UINT nCurIdx = 0;
	UINT nKeyFrameCnt = 0;

	std::vector<float> KeyFrameTimes;
	std::vector<SoundKeyFrame> SoundKeyFrames;
};


// n개의 오브젝트가 참조됨
// x개의 키프레임
// 모든 키프레임에 n개의 오브젝트의 위치가 반드시 존재
// 새로운 키프레임 추가 시 이전 키프레임의 내용을 모두 복사한 후 갱신/수정하는 형식으로 

class Cinematic
{
public:
	Cinematic();
	Cinematic(const Cinematic& rhs) = delete;
	Cinematic& operator=(const Cinematic& rhs) = delete;
	virtual ~Cinematic();

public:
	void Play();
	void Stop();
	void Update(float elapsedTime);
	void Destroy();

	void AddCamera(std::shared_ptr<Camera> pCamera, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation);
	void AddTrack(std::shared_ptr<Object> pObject, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale);
	void AddKeyFrame(UINT nTrackIdx, float time, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale, void(Object::*pFunction)());
	void AddCameraKeyFrame(float time, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation);

	void AddSoundKeyFrame(float time, const char* pstrFilePath, float volume);

	void MoveCameraKeyFrameData(UINT nCurIdx);
	void MoveKeyFrameData(UINT nTrackIdx, UINT nCurIdx);

protected:
	bool m_bPlay = false;
	bool m_bCinematicEnd = false;
	float m_ElapsedTime = 0.0f;
	float m_EndTime = 0.0f;
	
	std::vector<CinematicTrack> m_vCinematicTracks;
	CinematicCameraTrack m_CameraTrack;
	CinematicSoundTrack m_SoundTrack;


public:
	bool GetCinematicEnd() { return m_bCinematicEnd; }

};

#endif