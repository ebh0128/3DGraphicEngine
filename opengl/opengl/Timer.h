#pragma once
#pragma warning(disable:4996)

//델타타임을 찾아내는 타이머
//시스템 시간과 게임 시간을 구분했음
class Timer
{
protected:
	float m_dTimeGame;
	float m_dTimeSys;
	bool m_bGameTimer;
	bool m_bSysTimer;
	int m_OldTime;
	float m_TimeControl;
	int m_TimePause;


	void SetEngineTime();

public:
	void Init();
	void Release();
	void Update();
	float GetGameTime();
	float GetSysTime();
};