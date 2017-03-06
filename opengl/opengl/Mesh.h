#pragma once



typedef struct Material
{
	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec4 specular;
	GLfloat shininess;
	
};


class Sampler;
class Texture;
// ������ �׷����� ������Ʈ(BaseClass)
// ���� �ؽ��� �ڵ� ���� ����
// Assimp �𵨰� ȣȯ ���� ������� �ʾ����� �ִ��� ��︱�� �ְ� ������
class Mesh
{

private:
	GLuint vao;
	GLuint ebo;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint instance_vbo;
	
	
	//�������� �ؽ��� �ڵ� ���ü� ����
	GLuint vbo_texcoord[TEXCOORD_MAX];
	GLuint TexCoordCnt;

	Material* material;

	//���� ������ ���� (���߿� ������� ������ ������ ����ȭ)

	//Primitive Count(������Ʈ �ε�� �ڵ����)
	GLsizei PrimCount;
	GLsizei VerticesCount;

	std::vector<Sampler*> pMeshSamplerList;
	
	bool IsHaveNormal;
	bool IsHaveTexcoord;

private:
	//�� ����ϱ⶧���� �븻 ������ �ȵ�(�Ｎ���� ��� �ϱ�)
	//�ظ��ϸ� �븻 �Ѱ����� (���� 2�� + ���� + �븻������)
	GLfloat* NormalGenerate(GLfloat* vertices, GLuint* indices);

public:
	// ��ü �̸�
	char* Name;
	
	enum VBO_BUF_NAME { VBO_POSITION = 0 , VBO_NORMAL, NUM_VBOS};

	//default pos = 0 , norm = 1
	const GLuint attrib_Pos = 0;
	const GLuint attrib_Normal = 1;
	
	//���� Attrib��ȣ �� ���ķδ� attrib_Texcoord +1~+n
	const GLuint attrib_Texcoord =2;

	//�ν��Ͻ̿� 5~
	const GLuint attrib_MVPMat = 5;

public:
	Mesh();
	Mesh(GLfloat* vertices , int VertexNum , 
		GLuint* indices, int indicesNum, 
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);
	
	Mesh(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh);
	~Mesh();

	//�ؽ��� �ڵ�� �������� ���� �����ϹǷ�
	//��Ʈ����Ʈ Ȯ��� �̿��Ұ�
	void AddTexcoordAttibute(void* texcoords , int TexcoordNum);
	void SetMaterial(Material* pmat);
	Material* GetMaterial();
	
	void AddSampler(Sampler* pNewSamp);
	Sampler* CreateSampler(const char* ImagePath, GLuint Channel, GLint ShaderLoc , GLint DefShaderLoc = -500);

	//���α׷� ���ο��� ���� �޽� �ε�
	virtual void ObjectLoad(GLfloat* vertices, int VertexNum,
		GLuint* indices, int indicesNum,
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);

	//Assimp�� �̿��� �޽� �ε�
	virtual void ObjectLoad(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh);

	void MakeInstancingBuffer();
	void Render();
	void Render(glm::mat4 * MVPmats, unsigned int InstanceNum);

};


class SceneGL;
class Object;
//////////
///////// ������� ��� Ŭ����
class Node
{
protected:
	//� ������Ʈ�ΰ�
	Object* pObj;
	// ���� �ִ� ��
	SceneGL* pScene;
	// ���� ���� ������ ������Ʈ ()
	Node* Parent;
	std::vector<Node*> Children;
	std::vector<Mesh*> meshes;
	

	// ���� & ���� ��� ������Ʈ ����
	//(�ڽĳ���� ��� �θ�ȯ���� ��� ��ø)

	//�𵨰���
	glm::mat4 TransformMat;


	MyShader* pShader;

	MyShader* pDefGeoPass;
	MyShader* pDefLitPass;
	
	GLuint ubo;
	GLint UbSize;

public:
	char* Name;

	//���� �̿��ϴ��� ��庰�� Transform �����ϸ� ����
	glm::vec4 vPos;
	glm::vec3 vRot;
	glm::vec3 vScale;

	Node();
	Node(Node* _parent, SceneGL* scene);
	~Node();

	//���� 1��忡 1�޽����⶧���� �ʿ�(index ���� ���ϸ� 0���޽�)
	void SetMeshMaterial(Material* mat, int Index = 0);

	//������Ʈ���� ������ �ؼ� ����
	virtual void Update(GLfloat dtime);

	//�����������̳� ���̴� Ư���Ѱ� ����� ������ �ؾߵ�
	virtual void Render();

	//���۵� ������
//	virtual void DeferredRender();

	virtual void RenderGeoPass();
	virtual void RenderLitPass();

	virtual void ShaderParamInit();
	virtual void GeoPassInit();
	virtual void LitPassInit();

	void AddMesh(Mesh* pmesh);
	void AddChild(Node* pNode);
	void SetPosition(float x, float y, float z );
	void AddUBO(void* Data, GLuint Size, const char* BlockName, GLuint* Offset);
	void UpdateUBO(void* Data, GLuint Size, GLuint Offset);

	void SetObject(Object* Obj);
	glm::mat4 GetModelMat();
};


