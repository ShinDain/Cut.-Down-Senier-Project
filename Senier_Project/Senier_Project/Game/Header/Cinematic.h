#ifndef CINEMATIC_H

#define CINEMATIC_H

#include <vector>
#include <map>
#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/MathHelper.h"
#include "../../DirectXRendering/Header/Camera.h"
#include "../../DirectXRendering/Header/Object.h"

#define KEYFRAME_FUNCTION_EPSILON	0.00165f

// a Ű�����ӿ� p������Ʈ�� Ư�� �̵�/ȸ������ �����ȴ�.
// Ư�� �ִϸ��̼��� �����Ű����,
// Ư�� �Լ��� �����Ű�� ���� ����
struct KeyFrame // map�� �ð��� �Բ� ��ġ
{
	XMFLOAT3 xmf3Position;
	XMFLOAT3 xmf3Rotation;
	XMFLOAT3 xmf3Scale;

	void(Object::*pFunction)();
};

struct SoundKeyFrame // map�� �ð��� �Բ� ��ġ
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


// n���� ������Ʈ�� ������
// x���� Ű������
// ��� Ű�����ӿ� n���� ������Ʈ�� ��ġ�� �ݵ�� ����
// ���ο� Ű������ �߰� �� ���� Ű�������� ������ ��� ������ �� ����/�����ϴ� �������� 

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