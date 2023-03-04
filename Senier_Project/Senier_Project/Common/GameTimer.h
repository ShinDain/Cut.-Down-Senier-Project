//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#pragma once

class GameTimer
{
public:
	GameTimer();

	// 초단위
	float TotalTime()const;
	float DeltaTime()const;

	void Reset();	// 메시지 루프 이전
	void Start();	// 정지가 풀릴 때
	void Stop();	// 정지 시킬 때
	void Tick();	// 매 프레임 호출

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

