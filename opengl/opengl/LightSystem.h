#pragma once
//빛ㅊ 인스턴스를 관리

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


	//빛 감쇠 정도
	LitAttnu Attnuation;
public:
	//테스트용 코드
	GLfloat RespawnHeight;
	GLfloat DropSpeed;

	LightInstance(Object* Ori, ObjectInstance* _Parents = nullptr);
	
	//빛의 범위를 계산
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