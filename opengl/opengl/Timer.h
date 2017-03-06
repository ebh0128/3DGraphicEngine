#pragma once
#pragma warning(disable:4996)

//��ŸŸ���� ã�Ƴ��� Ÿ�̸�
//�ý��� �ð��� ���� �ð��� ��������
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