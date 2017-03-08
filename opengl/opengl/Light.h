#pragma once
//빛 클래스
// Draw는 구 or Sprite 예정
//빛 노드


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


	//지오메트리 패스에서는 아무것도 안해야됨
	void RenderGeoPass() {}
	void GeoPassInit() {}

	//빛 패스에서 실제로 작동
	void RenderPointLitPass();
	void RenderDirLitPass();
	
	void RenderStencilPass();

	void DirLitPassInit();
	void PointLitPassInit();
	void ShaderParamInit();
	
};