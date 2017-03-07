#pragma once
//���� �ν��Ͻ��� ����

typedef struct LightAttnuation
{
	float Constant;
	float Linear;
	float exp;
}LitAttnu;
class LightInstance : public ObjectInstance
{
protected:
	glm::vec3 Diffuse;
	glm::vec3 Ambient;
	glm::vec3 Specular;


	//�� ���� ����
	LitAttnu Attnuation;
public:
	//�׽�Ʈ�� �ڵ�
	GLfloat RespawnHeight;
	GLfloat DropSpeed;

	LightInstance(Object* Ori, ObjectInstance* _Parents = nullptr);
	
	//���� ������ ���
	float CalcLightArea();

	void Update(float dtime);
	
	void SetDiff(glm::vec3 Dif) { Diffuse = Dif; }
	void SetAmbi(glm::vec3 Amb) { Ambient = Amb; }
	void SetSpec(glm::vec3 Spec) { Specular = Spec; }

	void SetAttnuation(LitAttnu atn) {Attnuation = atn;}

	glm::vec3 GetDif() { return Diffuse; }
	glm::vec3 GetAmbi() { return Ambient; }
	glm::vec3 GetSpec() { return Specular; }

	LitAttnu GetAttnuation() { return Attnuation; }
};

class LightSystem : public Object
{
protected:

public:
	LightSystem(Node* Root, Object* Parent, SceneGL* Sce);


	
	void AddLight(LightInstance* pnew);
	void Update(GLfloat dtime);
	void Render();


};