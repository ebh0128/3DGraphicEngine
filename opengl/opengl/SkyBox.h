#pragma once

//스카이박스
//큐브를 사용하지 않고 quad를이용
//텍스쳐 코드 변환을 통해 구현
class SkyBox : public Object
{
	typedef struct Vector
	{
		GLfloat x; GLfloat y; GLfloat z;
	}Vec;
	typedef struct IndexVector
	{
		GLuint A; GLuint B; GLuint C;
	}InVec;
	
	glm::vec4 Diffuse;
public:
	SkyBox();
	SkyBox(Object* parent, SceneGL* Scene);
	void Update(GLfloat dtime);
	void SetDiffuse(glm::vec4 Col);
	
	
	void Render();

	void RenderDepthRead();
	void GeoPassInit();
};