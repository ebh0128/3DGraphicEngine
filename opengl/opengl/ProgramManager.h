#pragma once
//싱글톤 클래스

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


	//쉐이더 매니저 인터페이스
	void SetCurrentShader(MyShader* Cur);
	GLuint GetChannelID();
	void ReleaseChannel(int Channel);

	bool IsShaderLock();
	void ShaderLock(MyShader *pShader);
	void ShaderLockRelease();


private:
	static ProgramManager* m_pInstance;

	ShaderManager* m_pShaderManager;

	//초기화 여기서
	ProgramManager();

};