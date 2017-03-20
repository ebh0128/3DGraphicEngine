#pragma once



typedef struct Material
{
	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec4 specular;
	GLfloat shininess;
	
};
#define NUM_BONES_PER_VERTEX 4

struct VertexBoneData
{
	GLuint IDs[NUM_BONES_PER_VERTEX];
	float Weights[NUM_BONES_PER_VERTEX];
	VertexBoneData()
	{
		Reset();
	}
	void Reset()
	{
		memset(IDs, 0, sizeof(IDs));
		memset(Weights, 0, sizeof(Weights));
	}
	void AddBoneData(GLuint BoneID, float Weight);
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

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
	int m_NumVertices;


	//Assimp�� �𵨷ε� �̹Ƿ� ����
	virtual void CreateAssimpModel(const aiScene* pAssimpScene, std::string FilePath);

public:
	Model();
	
	//�ϳ��� �޽������ �𵨷� ����Ҷ� �̿��Ұ�
	void CreateMeshEntry(GLfloat* vertices, int VertexNum,
		GLuint* indices, int indicesNum,
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);

	//Assimp���� �� �ε��Ҷ� 
	virtual void CreateModelFromFile(std::string FilePath, std::string FileName);
	void AddMesh(MeshEntry* pNewMesh);

	//Mat Location 0 = Diff , 1 = Ambi , 2 = Spec , 3 =  Shiness
	void Render(GLuint* MatLocation = nullptr);
	void RenderInstance(int InstanceObjNum, GLuint* MatLocation = nullptr);

	void MakeInstancingBuffer(int iternum = 1);
	void SetInstanceBufferData(glm::mat4 *pData, int Index);

	//GLuint GetInstanceCount() { return InstanceBufferCount; }
	void SetPrimitiveKind(GLuint PrimKind);


};

//�޽� �ϳ��� ����
class MeshEntry
{

private:
	GLuint vao;
	GLuint ebo;
	GLuint vbo_position;
	GLuint vbo_normal;

	////������ ����
	GLuint Bonebo;
	
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



	GLuint m_BaseVertex;
	GLuint m_BaseIndex;

private:
	//�� ����ϱ⶧���� �븻 ������ �ȵ�(�Ｎ���� ��� �ϱ�)
	//�ظ��ϸ� �븻 �Ѱ����� (���� 2�� + ���� + �븻������)
	GLfloat* NormalGenerate(GLfloat* vertices, GLuint* indices);

public:
	// ��ü �̸�
	char* Name;
	
	enum VBO_BUF_NAME { VBO_POSITION = 0 , VBO_NORMAL, NUM_VBOS};

	//default pos = 0 , norm = 1
	
	//���� Attrib��ȣ �� ���ķδ� attrib_Texcoord +1~+n
	const GLuint attrib_Texcoord =2;

	//�ν��Ͻ̿� 5~
	const GLuint attrib_MVPMat = 8;

	bool IsSkinning;
	GLuint PrimitiveKind;
public:

	MeshEntry();
	MeshEntry(GLfloat* vertices , int VertexNum , 
		GLuint* indices, int indicesNum, 
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);
	
	MeshEntry(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh, bool IsSkinned = false);
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
	virtual void ObjectLoad(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh , bool IsSkinned =false);

	void AddBoneData(std::vector<VertexBoneData>& Bones);

	void Render();
	void RenderInstance(int InstanceObjCount);

	void MakeInstancingBuffer(int iternum = 1);
	void SetInstanceBufferData(glm::mat4 *pData, int Index);

	void SetBaseVertex(int Base) { m_BaseVertex = Base; }
	int GetBaseVertex() { return m_BaseVertex; }
	void SetBaseIndex(int Base) { m_BaseIndex = Base; }
	int GetBaseIndex() { return m_BaseIndex; }

	void SetPrimitiveKind(GLuint PrimKind) { PrimitiveKind = PrimKind; }

};


class SceneGL;
class Object;
