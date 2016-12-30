#pragma once

//�⺻������ �ϴ°� ��ü�� ī�޶�� ������ ���� �ϱ⿡ ���
//���̴��� �Ű������� �Ѱ��ִ��� ����ķ� ������� �����ϻ�
//vFront �� dir�� 
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