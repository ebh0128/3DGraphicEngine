#pragma once
//�̱��� Ŭ����

class MyShader;
class ShaderManager;

class ProgramManager
{
public:
	static ProgramManager* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new ProgramManager();
		}
		return m_pInstance;
	}


	//���̴� �Ŵ��� �������̽�
	void SetCurrentShader(MyShader* Cur);
	GLuint GetChannelID();
	void ReleaseChannel(int Channel);

	bool IsShaderLock();
	void ShaderLock(MyShader *pShader);
	void ShaderLockRelease();


private:
	static ProgramManager* m_pInstance;

	ShaderManager* m_pShaderManager;

	//�ʱ�ȭ ���⼭
	ProgramManager();

};