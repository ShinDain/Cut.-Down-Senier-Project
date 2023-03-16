//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include <Windows.h>
#include "GameTimer.h"

GameTimer::GameTimer()
	: m_SecondsPerCount(0.0), m_DeltaTime(-1.0), m_BaseTime(0),
	m_PausedTime(0), m_PrevTime(0), m_CurrTime(0), m_Stopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((_LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0 / (double)countsPerSec;
}

// Reset()이 호출된 이후부터 경과시간을 출력, 정지된 시간은 카운트 되지 않는다.
float GameTimer::TotalTime()const
{
	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance 
	// m_StopTime - m_BaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from m_StopTime:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  m_BaseTime       m_StopTime        startTime     m_StopTime    m_CurrTime

	if (m_Stopped)
		return (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);

	// The distance m_CurrTime - m_BaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from m_CurrTime:  
	//
	//  (m_CurrTime - m_PausedTime) - m_BaseTime 
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  m_BaseTime       m_StopTime        startTime     m_CurrTime

	else
		return (float)(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
}

float GameTimer::DeltaTime()const
{
	return (float)m_DeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_Stopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  m_BaseTime       m_StopTime        startTime   


	if (m_Stopped)
	{
		m_PausedTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_Stopped = false;
	}

}

void GameTimer::Stop()
{
	if (!m_Stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_StopTime = currTime;
		m_Stopped = true;
	}
}

void GameTimer::Tick()
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0f;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;

	// 프레임 사이의 경과시간
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;

	// 다음 프레임 준비
	m_PrevTime = m_CurrTime;

	// 강제로 양수화, 몇몇 프로세서는 절전모드 시 음수가 되는 경우 존재
	if (m_DeltaTime < 0.0f)
		m_DeltaTime = 0.0f;
}