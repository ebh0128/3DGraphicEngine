#pragma once
//빛 클래스
// Draw는 구 or Sprite 예정
//빛 노드



class DirLight :public Object
{
	friend class SceneGL;

	typedef struct Vector
	{
		GLfloat x; GLfloat y; GLfloat z;
	}Vec;
	typedef struct IndexVector
	{
		GLuint A; GLuint B; GLuint C;
	}InVec;
protected:
	glm::vec3 Diffuse;
	glm::vec3 Ambient;
	glm::vec3 Specular;

	LitAttnu Attnu;

	std::string SSAOShaderName;
	std::string BlurShaderName;
	std::string HDRShaderName;
	std::string ShadowShaderName;

	MyShader* m_pShaderSSAO;
	MyShader* m_pShaderBlur;
	MyShader* m_pShaderHDR;
	MyShader* m_pShaderShadow;

	float Kernel[KERNEL_SIZE*3];

	GLuint NosieTexture;

	glm::mat4 LightOrtho;
	glm::mat4 LightView;

public:
	DirLight();
	DirLight(Object* parent, SceneGL* Scene);

	void SetPos(glm::vec4 Lightpos);
	void SetDiffuse(glm::vec3 dif);
	void SetSpec(glm::vec3 dif);

	//dir 에선 방향임
	//glm::vec4 GetPos() { return vPos; }
	glm::vec3 GetDif() { return Diffuse; }
	glm::vec3 GetAmb() { return Ambient; }
	glm::vec3 GetSpec() { return Specular; }

	glm::mat4 GetLightVPMat() { return LightOrtho * LightView; }

	GLuint GetNoiseTexture() { return NosieTexture; }
	float* GetKernel() { return Kernel; }

	void Update(GLfloat dtime);
	void Render();
	void ShaderParamInit(MyShader* ManagedShader = nullptr);

	void InitKernel();
};

