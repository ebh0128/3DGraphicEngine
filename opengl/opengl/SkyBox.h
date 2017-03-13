#pragma once

//��ī�̹ڽ�
//ť�긦 ������� �ʰ� quad���̿�
//�ؽ��� �ڵ� ��ȯ�� ���� ����
class SkyBox : public Node
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
	SkyBox(Node* parent, SceneGL* Scene);
	void Update(GLfloat dtime);
	void SetDiffuse(glm::vec4 Col);
	
	
	void Render();

	void RenderDepthRead();
	void GeoPassInit();
};