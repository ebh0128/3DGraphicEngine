#include "CommonHeader.h"
#include "myShader.h"
#include "ProgramManager.h"

ProgramManager::ProgramManager()
{
	m_pShaderManager = new ShaderManager();

}

void ProgramManager::SetCurrentShader(MyShader* Cur)
{
	m_pShaderManager->SetCurrentShader(Cur);
}
GLuint ProgramManager::GetChannelID()
{
	return m_pShaderManager->GetChannelID();
}
void ProgramManager::ReleaseChannel(int Channel)
{
	m_pShaderManager->ReleaseChannel(Channel);
}

bool ProgramManager::IsShaderLock()
{
	return m_pShaderManager->CheckLock();
}
void  ProgramManager::ShaderLock(MyShader *pShader)
{
	m_pShaderManager->LockShader(pShader);
}
void  ProgramManager::ShaderLockRelease()
{
	m_pShaderManager->ReleaseLock();
}