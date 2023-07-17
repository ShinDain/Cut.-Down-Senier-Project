#ifndef SOUND_H

#define SOUND_H

#include "../../Common/Header/D3DUtil.h"
#include "../../DirectXRendering/Header/Global.h"

class Sound
{
//public:
//	Sound();
//	Sound(const Sound& rhs) = delete;
//	Sound& operator=(const Sound& rhs) = delete;
//	virtual ~Sound();

public:
    static void PlaySoundFile(const wchar_t* pstrFileName, bool bIsWav);

	static void PlayBGM(const wchar_t* pstrFileName, bool bIsWav);
	static void StopBGM();
};

#endif
