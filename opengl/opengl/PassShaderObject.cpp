#include "CommonHeader.h"
#include "PassShaderObject.h"


PassShaderObject::PassShaderObject(MyShader *passShader)
{
	m_pPassShader = passShader;
}
void PassShaderObject::AddPassObject(Object* obj)
{

}
//이 쉐이더의 이름과 똑같으면 true반환
bool PassShaderObject::CheckName(char* newName)
{
	//디버그 위해
	char ShaderName[256];
	m_pPassShader->GetName(ShaderName);
	bool ret = !strcmp(newName, ShaderName);
	return ret;
}
bool PassShaderObject::CheckName(std::string newName)
{
	//디버그 위해
	//char ShaderName[256];
	//m_pPassShader->GetName(ShaderName);
	
	std::string  ShaderName = m_pPassShader->GetNameString();
	bool ret = !strcmp(newName.c_str(), ShaderName.c_str());
	return ret;
}
void PassShaderObject::GetShaderName(char* retName)
{
	m_pPassShader->GetName(retName);
}
MyShader* PassShaderObject::ApplyPassShader()
{
	//if (!m_pPassShader) return;
	m_pPassShader->ApplyShader();
	return m_pPassShader;
}
std::string PassShaderObject::GetNameString()
{
	return m_pPassShader->GetNameString();
}
