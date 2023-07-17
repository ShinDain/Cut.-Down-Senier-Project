#include "../Header/Sound.h"

void Sound::PlaySoundFile(const wchar_t* pstrFileName, bool bIsWav)
{
	wchar_t pstr[256] = L"open ";
	// PLAY
	wcscpy_s(pstr, L"play ");
	wcscat_s(pstr, pstrFileName);
	//wcscat_s(pstr, L" wait");
	int error = mciSendString((pstr), 0, 0, 0);
}

void Sound::PlayBGM(const wchar_t* pstrFileName, bool bIsWav)
{
	PlaySound(pstrFileName, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void Sound::StopBGM()
{
	PlaySound(NULL, NULL, SND_FILENAME | SND_ASYNC);
}

