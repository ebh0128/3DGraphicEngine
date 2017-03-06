#pragma once
//빛 클래스
// Draw는 구 or Sprite 예정



class Light :public Node
{
	friend class SceneGL;
protected:
	glm::vec3 Diffuse;
	glm::vec3 Ambient;
	glm::vec3 Specular;

	GLfloat RespawnHeight;
	GLfloat DropSpeed;

public:
	Light();
	Light(Node* parent, SceneGL* Scene);

	void SetPos(glm::vec4 Lightpos);
	void SetDiffuse(glm::vec3 dif);
	void SetSpec(glm::vec3 dif);
	void SetRespawHeigt(GLfloat Height);
	
	void Update(GLfloat dtime);
	void Render();

	//쉐이더에 빛 속성값들을 넘겨준다(매개 변수는 빛 적용할 오브젝트의 쉐이더)
	
};