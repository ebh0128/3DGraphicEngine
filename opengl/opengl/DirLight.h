#pragma once
//�� Ŭ����
// Draw�� �� or Sprite ����
//�� ���



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

	//dir ���� ������
	//glm::vec4 GetPos() { return vPos; }
	glm::vec3 GetDif() { return Diffuse; }
	glm::vec3 GetAmb() { return Ambient; }
	glm::vec3 GetSpec() { return Specular; }

	glm::mat4 GetLightVPMat() { return LightOrtho * LightView; }

	void Update(GLfloat dtime);
	void Render();

	//������Ʈ�� �н������� �ƹ��͵� ���ؾߵ�
	void RenderGeoPass() {}
	void GeoPassInit(MyShader* ManagedShader = nullptr) {}

	//�� �н����� ������ �۵�
	void RenderPointLitPass() {}
	void RenderDirLitPass();

	void DirLitPassInit(MyShader* ManagedShader = nullptr);
	void PointLitPassInit(MyShader* ManagedShader = nullptr) {}
	void ShaderParamInit(MyShader* ManagedShader = nullptr);

	//��Ÿ �н�
	void SSAOPass();
	void BlurPass();

	void HDRPass();

	void InitKernel();
};

