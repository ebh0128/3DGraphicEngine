#include"CommonHeader.h"
#include "Camera.h"
#include "DroneCamera.h"
#include "myShader.h"
#include "SpotLight.h"


SpotLight::SpotLight()
{

}
SpotLight::SpotLight(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up, glm::vec4 Diffuse,
	float _ConeAngle, float _fLinearAtt) : DroneCamera(Pos , Lookat , Up)
{
	vDiffuse = Diffuse;
	ConeCosine = cosf(_ConeAngle);
	fLinearAtt = _fLinearAtt;

}

void SpotLight::TransferToShader(MyShader* pShader)
{
	pShader->ApplyShader();
	pShader->SetUniform4fv("sLight.Diffuse", glm::value_ptr(vDiffuse));
	pShader->SetUniform4fv("sLight.vPosition", glm::value_ptr(vPosition));
	pShader->SetUniform4fv("sLight.vDirection", glm::value_ptr(vFront));
	pShader->SetUniform1f("sLight.fConeCosine", ConeCosine);
	pShader->SetUniform1f("sLight.fLinearAttnuation", fLinearAtt);

}