#ifndef CINEMATIC_H

#define CINEMATIC_H

#include <vector>
#include <map>
#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Camera.h"
#include "../../DirectXRendering/Header/Object.h"

// a Ű�����ӿ� p������Ʈ�� Ư�� �̵�/ȸ������ �����ȴ�.
// Ư�� �ִϸ��̼��� �����Ű����,
// Ư�� �Լ��� �����Ű�� ���� ����
struct KeyFrame // map�� �ð��� �Բ� ��ġ
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