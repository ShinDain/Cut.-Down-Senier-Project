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
	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;

	bool m_Stopped;

};

