#pragma once


class SceneGL;
class Node;
class ObjectInstance;
class MeshEntry;
class MyShader;
class Model;

//���� ���ÿ� ������Ÿ�� �������� �������
class Object
{
protected:
	Node* pRoot;
	SceneGL* pScene;
	Object* mParent;
	 
	//�޽� ����
	Model*m_pModel;

	std::vector<ObjectInstance*> InstanceList;

	//Render �Ҷ� ����ϴ� �ν��Ͻ� ��� ����
	std::vector<glm::mat4> MatrixList;

	//�ʿ�������� ������ �ν��Ͻ����� �ڽ� ������ ����?
	std::vector<Object*> ChildList;

	//�𵨰���
	glm::mat4 TransformMat;


	ShaderManager* m_pShaderManager;
	//�̵� ����
//	MyShader* pShader;
	std::string ForwardShaderName;
	
	
	//MyShader* pDefGeoPass;
	std::string GeoShaderName;

	
	//����Ʈ ����Ʈ
	//MyShader* pDefPtLitPass;
	std::string PTLightShaderName;

	//�𷺼� ����Ʈ
	//MyShader* pDefDirLitPass;
	std::string DirLightShaderName;

	//MyShader* m_pShaderShadow;
	std::string ShadowShaderName;

	//�� ������ �Ѱ��ֱ����� ����(�����־�ߵǴ��� ����� �ʿ�)
	GLuint ubo;
	GLint UbSize;

	//�� ����
	glm::vec4 vPos;
	glm::vec3 vRot;
	glm::vec3 vScale;

	GLuint MainTextureUnit;

	GLboolean m_bUpdated;

	GLuint MatLocArray[4];
public:
	//��Ʈ ��带 �����ϰ� ����
	Object();
	Object(Object* Parent , SceneGL* Sce);
	
	//���⼭ ���̴� �ʱ�ȭ �Ұ�
	virtual void ShaderInit() {}
	
//	Object(MeshEntry* BaseMesh, Object* Parent, SceneGL* Sce);
//	Object(MeshEntry* BaseMesh, Object* Parent, SceneGL* Sce);

	void AddInstance(ObjectInstance* TsetInfo);

	virtual void AddInstanceCallBack() {}
	virtual void Update(GLfloat dtime);
	virtual void Render();
	


	virtual void ShaderParamInit(MyShader* ManagedShader = nullptr);
	//���ο��� ���̴� ����, �ʱ�ȭ�� ���� , �ܺο��� ����ߵ�
	virtual void RenderByPipeLine();

	
	//�߰��� �ν��Ͻ��� ���ٸ� �ݵ�� ������ �ؼ� �߰��� ������ ��
	// �⺻ �޽��� World Instance �� ����
	virtual void InstanceDataSetting();

	void AddChild(Object* pChild);
	Node* GetRoot();
	int GetInstanceNum();
	glm::mat4* GetInstanceMatrixData();
	glm::mat4 GetModelMat();
	void MatrixListInit(int index = 0);
	//�� ������ �޾ƺ������� ������ ���
	//�� ������ �ʿ��ϸ� ��ߵ�
	void AddUBO(void* Data, GLuint Size, const char* BlockName, GLuint* Offset, MyShader* pshad);
	void UpdateUBO(void* Data, GLuint Size, GLuint Offset);

	void SetPosition(float x, float y, float z);
	glm::vec4 GetPos() { return vPos; }

	GLuint GetMainTextureUnit() { return MainTextureUnit; }

};

