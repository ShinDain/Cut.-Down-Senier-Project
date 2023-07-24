#ifndef SOUND_H

#define SOUND_H

#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Global.h"
#include "../../DirectXRendering/Header/Object.h"

// FMOD
//#include "../../FMOD/inc/fmod.h"
#include "../../FMOD/inc/fmod.hpp"


#define VOLUME_MAX 1.0f
#define VOLUME_MIN 0.0f
#define VOLUME_DEFAULT 0.3f
#define VOLUME_WEIGHT 0.1f;

class CSound
{
public:
	CSound(const char* pstrFilePath, bool bLoop);
	CSound(const CSound& rhs) = delete;
	CSound& operator=(const CSound& rhs) = delete;
	virtual ~CSound();

public:

	static void Init();
	static void Release();

	void Update();

    void Play();
	void Pause();
	void Resume();
	void Stop();
	void VolumeUp();
	void VolumeDown();

	void SetVolme(float volumeSize);
	void SetPitch(float pitchSize);

protected:
	static FMOD::System* g_pSystem;
	FMOD::Channel* m_pChannel = nullptr;
	FMOD::Sound* m_pSound = nullptr;

	float m_Volume = VOLUME_DEFAULT;
	bool m_bIsPlaying;

public:
	bool GetIsPlaying() { return m_bIsPlaying; }

};

#endif
