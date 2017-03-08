#pragma once
//�� Ŭ����
// Draw�� �� or Sprite ����
//�� ���


class Light :public Node
{
	friend class SceneGL;
protected:
	glm::vec3 Diffuse;
	glm::vec3 Ambient;
	glm::vec3 Specular;

	GLfloat RespawnHeight;
	GLfloat DropSpeed;

	MyShader* nullShader;
public:
	Light();
	Light(Node* parent, SceneGL* Scene);

	void SetPos(glm::vec4 Lightpos);
	void SetDiffuse(glm::vec3 dif);
	void SetSpec(glm::vec3 dif);
	void SetRespawHeigt(GLfloat Height);

	void Update(GLfloat dtime);
	void Render();


	//������Ʈ�� �н������� �ƹ��͵� ���ؾߵ�
	void RenderGeoPass() {}
	void GeoPassInit() {}

	//�� �н����� ������ �۵�
	void RenderPointLitPass();
	void RenderDirLitPass();
	
	void RenderStencilPass();

	void DirLitPassInit();
	void PointLitPassInit();
	void ShaderParamInit();
	
};