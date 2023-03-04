//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#pragma once

class GameTimer
{
public:
	GameTimer();

	// �ʴ���
	float TotalTime()const;
	float DeltaTime()const;

	void Reset();	// �޽��� ���� ����
	void Start();	// ������ Ǯ�� ��
	void Stop();	// ���� ��ų ��
	void Tick();	// �� ������ ȣ��

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;

};

