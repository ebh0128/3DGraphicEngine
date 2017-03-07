#pragma once


class SceneGL;
class Node;

//���� ���ÿ� ������Ÿ�� �������� �������
class Object
{
protected:
	Node* pRoot;
	SceneGL* pScene;
	Object* mParent;

	std::vector<TransSet*> InstanceList;
	std::vector<glm::mat4> MatrixList;

	std::vector<Object*> ChildList;
public:
	//��Ʈ ��带 �����ϰ� ����
	Object(Node* Root,Object* Parent , SceneGL* Sce);
	void AddInstance(TransSet* TsetInfo);
	virtual void Update(GLfloat dtime);
	virtual void Render();

	virtual void RenderGeoPass();
	virtual void RenderLitPass();

	void AddChild(Object* pChild);
	Node* GetRoot();
	int GetInstanceNum();
	glm::mat4* GetInstanceMatrixData();
};