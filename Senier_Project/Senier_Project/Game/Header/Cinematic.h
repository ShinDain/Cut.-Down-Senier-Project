#ifndef CINEMATIC_H

#define CINEMATIC_H

#include <vector>
#include <map>
#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Camera.h"
#include "../../DirectXRendering/Header/Object.h"

// a 키프레임에 p오브젝트가 특정 이동/회전으로 조정된다.
// 특정 애니메이션을 재생시키려면,
// 특정 함수를 재생시키는 것이 유리
struct KeyFrame // map에 시간과 함께 배치
{
	XMFLOAT3 xmf3Position;
	XMFLOAT3 xmf3Rotation;
	XMFLOAT4 xmf4Orientation;
	XMFLOAT3 xmf3Scale;
			 
	void(*pFunction)();
};

struct CinematicTrack
{
	std::shared_ptr<Object> pObject;

	UINT nCurIdx = 0;
	UINT nKeyFrameCnt = 0;

	std::vector<float> KeyFrameTimes;
	std::vector<KeyFrame> KeyFrames;
};

struct CinematicCameraTrack
{
	std::shared_ptr<Camera> pCamera;

	UINT nCurIdx = 0;
	UINT nKeyFrameCnt = 0;

	std::vector<float> KeyFrameTimes;
	std::vector<KeyFrame> KeyFrames;
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
	void Update(float elapsedTime);

	void AddTrack(std::shared_ptr<Object> pObject);
	void AddKeyFrame(UINT nTrackIdx, float time, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale, void(*pFunction)());
	void AddCameraKeyFrame(float time, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation, XMFLOAT3 xmf3Scale);

	void GetCameraKeyFrameData(UINT nCurIdx);
	void GetKeyFrameData(UINT nCurIdx);

protected:
	bool m_bPlay = false;
	bool m_bCinematicEnd = false;
	float m_ElapsedTime = 0.0f;
	float m_EndTime = 0.0f;
	
	std::vector<CinematicTrack> m_vCinematicTracks;
	CinematicCameraTrack m_CameraTrack;


public:
	void SetCamera(std::shared_ptr<Camera> pCamera);


	bool GetCinematicEnd() { return m_bCinematicEnd; }

};

#endif