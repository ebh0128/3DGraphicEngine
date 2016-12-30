#include "CommonHeader.h"
#include "Timer.h"
#include "MMSystem.h"


void Timer::Init()
{
	m_dTimeGame = 0;
	m_dTimeSys = 0;
	m_bGameTimer = false;
	m_bSysTimer = false;
	m_TimePause = 1;
	m_OldTime = timeGetTime();
	timeBeginPeriod(1);
	m_TimeControl = 0;

}
void Timer::SetEngineTime()
{
	int NewTime = timeGetTime();
	float dTime = (NewTime -  m_OldTime) * 0.001f;
	m_OldTime = NewTime;
	
	//입력신호 처리에서 m_TimeControl, m_TimePause 처리 만들것
	m_dTimeGame = dTime*(m_TimeControl+1)*m_TimePause;
	m_dTimeSys = dTime;
	
}
void Timer::Release()
{	
	timeEndPeriod(1);
}
void Timer::Update()
{
	
	SetEngineTime();
}
float Timer::GetGameTime()
{
	return m_dTimeGame;
}
float Timer::GetSysTime()
{
	return m_dTimeSys;
}
