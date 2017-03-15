#pragma once
//빛ㅊ 인스턴스를 관리

class ObjectInstance;

class LightInstance : public ObjectInstance
{
protected:
	glm::vec3 Diffuse;
	glm::vec3 Ambient;
	glm::vec3 Specular;

	
	//빛 감쇠 정도
	LitAttnu Attnuation;
	
public:
	//로직에 크게 영향없는 게임 플레이변수(나중에 Get Set 만들수도 있음)
	GLfloat RespawnHeight;
	GLfloat DropSpeed;

	//빛 요소들의 세기
	float DiffuseFactor;
	float AmbientFactor;
	float SpecularFactor;
	//////////////////////////

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

//
class LightSystem : public Object
{
protected:
	std::string NullShaderName;
	MyShader* nullShader;

public:
	LightSystem(Object* Parent, SceneGL* Sce);

	void AddLight(LightInstance* pnew);

	std::vector<glm::mat4> LightAttributeList;
	void AddInstanceCallBack();

	
	void Update(GLfloat dtime);
	
	void Render();

	//빛
	void RenderPointLitPass();
	void RenderStencilPass();

	void RenderGeoPass() {}
	void GeoPassInit(MyShader* ManagedShader = nullptr) {}

	void PointLitPassInit(MyShader* ManagedShader = nullptr);
	void ShaderParamInit(MyShader* ManagedShader = nullptr);

	virtual void InstanceDataSetting();

};

