#pragma once

//��ī�̹ڽ�
//ť�긦 ������� �ʰ� quad���̿�
//�ؽ��� �ڵ� ��ȯ�� ���� ����
class SkyBox : public Object
{
	
	
	glm::vec4 Diffuse;
public:
	SkyBox();
	SkyBox(Object* parent, SceneGL* Scene);
	void Update(GLfloat dtime);
	void SetDiffuse(glm::vec4 Col);
	
	
	void Render();
	void RenderDepthRead();

};