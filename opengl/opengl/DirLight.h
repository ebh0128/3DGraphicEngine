#pragma once
//빛 클래스
// Draw는 구 or Sprite 예정
//빛 노드


class DirLight :public Node
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

	float Kernel[KERNEL_SIZE*3];

	GLuint NosieTexture;

public:
	DirLight();
	DirLight(Node* parent, SceneGL* Scene);

	void SetPos(glm::vec4 Lightpos);
	void SetDiffuse(glm::vec3 dif);
	void SetSpec(glm::vec3 dif);

	//dir 에선 방향임
	glm::vec4 GetPos() { return vPos; }
	glm::vec3 GetDif() { return Diffuse; }
	glm::vec3 GetAmb() { return Ambient; }
	glm::vec3 GetSpec() { return Specular; }

	void Update(GLfloat dtime);
	void Render();

	//지오메트리 패스에서는 아무것도 안해야됨
	void RenderGeoPass() {}
	void GeoPassInit() {}

	//빛 패스에서 실제로 작동
	void RenderPointLitPass() {}
	void RenderDirLitPass();

	void DirLitPassInit();
	void PointLitPassInit() {}
	void ShaderParamInit();

	//기타 패스
	void SSAOPass();
	void BlurPass();

	void HDRPass();

	void InitKernel();
};