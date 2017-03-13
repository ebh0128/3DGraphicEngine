#pragma once


class SceneGL;
class Node;
class ObjectInstance;
class MeshEntry;
class MyShader;

//���� ���ÿ� ������Ÿ�� �������� �������
class Object
{
protected:
	Node* pRoot;
	SceneGL* pScene;
	Object* mParent;

	//�޽� ����
	std::vector<MeshEntry*> meshes;

	std::vector<ObjectInstance*> InstanceList;

	//Render �Ҷ� ����ϴ� �ν��Ͻ� ��� ����
	std::vector<glm::mat4> MatrixList;

	//�ʿ�������� ������ �ν��Ͻ����� �ڽ� ������ ����?
	std::vector<Object*> ChildList;

	//�𵨰���
	glm::mat4 TransformMat;

	//�̵� ����
	MyShader* pShader;
	MyShader* pDefGeoPass;
	//����Ʈ ����Ʈ
	MyShader* pDefPtLitPass;
	//�𷺼� ����Ʈ
	MyShader* pDefDirLitPass;
	MyShader* m_pShaderShadow;

	//�� ������ �Ѱ��ֱ����� ����(�����־�ߵǴ��� ����� �ʿ�)
	GLuint ubo;
	GLint UbSize;

	//�� ����
	glm::vec4 vPos;
	glm::vec3 vRot;
	glm::vec3 vScale;


public:
	//��Ʈ ��带 �����ϰ� ����
	Object(Node* Root,Object* Parent , SceneGL* Sce);
//	Object(MeshEntry* BaseMesh, Object* Parent, SceneGL* Sce);
//	Object(MeshEntry* BaseMesh, Object* Parent, SceneGL* Sce);

	void AddInstance(ObjectInstance* TsetInfo);
	virtual void Update(GLfloat dtime);
	virtual void Render();

	virtual void RenderGeoPass();
	//����Ʈ �н�
	virtual void RenderPointLitPass() {}
	virtual void RenderDirLitPass() {}

	virtual void RenderShadowPass();

	void AddChild(Object* pChild);
	Node* GetRoot();
	int GetInstanceNum();
	glm::mat4* GetInstanceMatrixData();
};