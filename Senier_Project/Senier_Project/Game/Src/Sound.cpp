#include "../Header/Sound.h"

FMOD::System* CSound::g_pSystem(nullptr);

CSound::CSound(const char* pstrFilePath, bool bLoop)
{
	if (bLoop)
	{
		g_pSystem->createSound(pstrFilePath, FMOD_LOOP_NORMAL, 0, &m_pSound);
	}
	else
	{
		g_pSystem->createSound(pstrFilePath, FMOD_LOOP_OFF, 0, &m_pSound);
	}

	m_pChannel = nullptr;
	m_Volume = VOLUME_DEFAULT;
}

CSound::~CSound()
{
	m_pSound->release();
}

void CSound::Init()
{
	FMOD_RESULT res;
	res = FMOD::System_Create(&g_pSystem);
	if (res != FMOD_OK) return;
	res = g_pSystem->init(32, FMOD_INIT_NORMAL, 0);
	if (res != FMOD_OK) return;

}

void CSound::Release()
{
	g_pSystem->close();
	g_pSystem->release();
}

void CSound::Update()
{
	m_pChannel->isPlaying(&m_bIsPlaying);

	if(m_bIsPlaying)
		g_pSystem->update();
}

void CSound::Play()
{
	g_pSystem->playSound(m_pSound, 0, false, &m_pChannel);
}

void CSound::Pause()
{
	m_pChannel->setPaused(true);
}

void CSound::Resume()
{
	m_pChannel->setPaused(false);
}

void CSound::Stop()
{
	m_pChannel->stop();
}

void CSound::VolumeUp()
{
	if (m_Volume < VOLUME_MAX)
		m_Volume += VOLUME_WEIGHT;

	m_pChannel->setVolume(m_Volume);
}

void CSound::VolumeDown()
{
	if (m_Volume > VOLUME_MIN)
		m_Volume -= VOLUME_WEIGHT;

	m_pChannel->setVolume(m_Volume);
}

void CSound::SetVolme(float volumeSize)
{
	m_Volume = volumeSize;

	if (m_Volume < VOLUME_MIN)
		m_Volume = 0.0f;

	if (m_Volume > VOLUME_MAX)
		m_Volume = 1.0f;

	m_pChannel->setVolume(m_Volume);
}
