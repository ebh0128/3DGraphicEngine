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
class MeshEntry;
class MyShader;
// ���� ����ϴ� �� �޽� ���
class Model
{
protected:
	std::vector<MeshEntry*> m_MeshList;
	GLuint m_MainTextureUnitNum;

//	GLuint instance_vbo[10];
//	GLuint InstanceBufferCount;
//	glm::mat4* InstanceBufferData[10];

//	const GLuint attrib_MVPMat = 5;

public:
	Model();
	//Assimp�� �𵨷ε� �̹Ƿ� ����
	void CreateAssimpModel(const aiScene* pAssimpScene, std::string FilePath);

	//�ϳ��� �޽������ �𵨷� ����Ҷ� �̿��Ұ�
	void CreateMeshEntry(GLfloat* vertices, int VertexNum,
		GLuint* indices, int indicesNum,
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);

	//Assimp���� �� �ε��Ҷ� 
	void CreateModelFromFile(std::string FilePath, std::string FileName);
	void AddMesh(MeshEntry* pNewMesh);

	//Mat Location 0 = Diff , 1 = Ambi , 2 = Spec , 3 =  Shiness
	void Render(GLuint* MatLocation = nullptr);
	void RenderInstance(int InstanceObjNum, GLuint* MatLocation = nullptr);

	void MakeInstancingBuffer(int iternum = 1);
	void SetInstanceBufferData(glm::mat4 *pData, int Index);

	//GLuint GetInstanceCount() { return InstanceBufferCount; }


};

//�޽� �ϳ��� ����
class MeshEntry
{

private:
	GLuint vao;
	GLuint ebo;
	GLuint vbo_position;
	GLuint vbo_normal;


	
	//�������� �ؽ��� �ڵ� ���ü� ����
	GLuint vbo_texcoord[TEXCOORD_MAX];
	GLuint TexCoordCnt;

	Material* material;

	//���� ������ ���� (���߿� ������� ������ ������ ����ȭ)

	//Primitive Count(������Ʈ �ε�� �ڵ����)
	GLsizei PrimCount;
	GLsizei VerticesCount;

	//std::vector<Sampler*> pMeshSamplerList;
	std::vector<Texture*> pTextureList;

	bool IsHaveNormal;
	bool IsHaveTexcoord;

	GLuint instance_vbo[10];
	GLuint InstanceBufferCount;
	glm::mat4* InstanceBufferData[10];


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
	MeshEntry();
	MeshEntry(GLfloat* vertices , int VertexNum , 
		GLuint* indices, int indicesNum, 
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);
	
	MeshEntry(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh);
	~MeshEntry();

	//�ؽ��� �ڵ�� �������� ���� �����ϹǷ�
	//��Ʈ����Ʈ Ȯ��� �̿��Ұ�
	void AddTexcoordAttibute(void* texcoords , int TexcoordNum);
	void SetMaterial(Material* pmat);
	Material* GetMaterial();
	
	void AddTexture(Texture* pNewTex);

	//���α׷� ���ο��� ���� �޽� �ε�
	virtual void ObjectLoad(GLfloat* vertices, int VertexNum,
		GLuint* indices, int indicesNum,
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);

	//Assimp�� �̿��� �޽� �ε�
	virtual void ObjectLoad(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh);

	void Render();
	void RenderInstance(int InstanceObjCount);

	void MakeInstancingBuffer(int iternum = 1);
	void SetInstanceBufferData(glm::mat4 *pData, int Index);

};


class SceneGL;
class Object;
