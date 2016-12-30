#pragma once

//기본적으로 하는거 자체는 카메라와 완전히 동일 하기에 상속
//쉐이더에 매개변수를 넘겨주느냐 뷰행렬로 만드냐의 차이일뿐
//vFront 가 dir로 
class SpotLight : public DroneCamera
{
protected:
	glm::vec4 vDiffuse;
	float ConeCosine;
	float fLinearAtt;

public:
	SpotLight();
	SpotLight(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up , glm::vec4 Diffuse,
		float _ConeAngle , float _fLinearAtt);

	void TransferToShader(MyShader* pShader);
};