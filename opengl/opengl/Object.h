#pragma once


class SceneGL;
class Node;
class ObjectInstance;
//���� ���ÿ� ������Ÿ�� �������� �������
class Object
{
protected:
	Node* pRoot;
	SceneGL* pScene;
	Object* mParent;

	//std::vector<TransSet*> InstanceList;
	std::vector<ObjectInstance*> InstanceList;

	//Render �Ҷ� ����ϴ� �ν��Ͻ� ��� ����
	std::vector<glm::mat4> MatrixList;

	std::vector<Object*> ChildList;
public:
	//��Ʈ ��带 �����ϰ� ����
	Object(Node* Root,Object* Parent , SceneGL* Sce);
	void AddInstance(ObjectInstance* TsetInfo);
	virtual void Update(GLfloat dtime);
	virtual void Render();

	virtual void RenderGeoPass();
	//����Ʈ �н�
	virtual void RenderPointLitPass() {}
	virtual void RenderDirLitPass() {}
	void AddChild(Object* pChild);
	Node* GetRoot();
	int GetInstanceNum();
	glm::mat4* GetInstanceMatrixData();
};