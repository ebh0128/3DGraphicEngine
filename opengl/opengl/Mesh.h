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
// 실제 사용하는 모델 메쉬 덩어리
class Model
{
protected:
	std::vector<MeshEntry*> m_MeshList;
	GLuint m_MainTextureUnitNum;

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
	int m_NumVertices;


	//Assimp용 모델로드 이므로 숨김
	virtual void CreateAssimpModel(const aiScene* pAssimpScene, std::string FilePath);

public:
	Model();
	
	//하나의 메쉬덩어리를 모델로 사용할때 이용할것
	void CreateMeshEntry(GLfloat* vertices, int VertexNum,
		GLuint* indices, int indicesNum,
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);

	//Assimp에서 모델 로드할때 
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

//메쉬 하나를 말함
class MeshEntry
{

private:
	GLuint vao;
	GLuint ebo;
	GLuint vbo_position;
	GLuint vbo_normal;

	////뼈정보 버퍼
	GLuint Bonebo;
	
	//여러개의 텍스쳐 코드 들어올수 있음
	GLuint vbo_texcoord[TEXCOORD_MAX];
	GLuint TexCoordCnt;

	Material* material;

	//정점 데이터 보관 (나중에 사용할일 없으면 지워서 최적화)

	//Primitive Count(오브젝트 로드시 자동계산)
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
	//빛 계산하기때문에 노말 없으면 안됨(즉석에서 계산 하기)
	//왠만하면 노말 넘겨주자 (뺄셈 2번 + 외적 + 노말라이즈)
	GLfloat* NormalGenerate(GLfloat* vertices, GLuint* indices);

public:
	// 객체 이름
	char* Name;
	
	enum VBO_BUF_NAME { VBO_POSITION = 0 , VBO_NORMAL, NUM_VBOS};

	//default pos = 0 , norm = 1
	
	//시작 Attrib번호 즉 이후로는 attrib_Texcoord +1~+n
	const GLuint attrib_Texcoord =2;

	//인스턴싱용 5~
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

	//텍스쳐 코드는 여러개가 존재 가능하므로
	//어트리뷰트 확장시 이용할것
	void AddTexcoordAttibute(void* texcoords , int TexcoordNum);
	void SetMaterial(Material* pmat);
	Material* GetMaterial();
	
	void AddTexture(Texture* pNewTex);

	//프로그램 내부에서 만든 메쉬 로드
	virtual void ObjectLoad(GLfloat* vertices, int VertexNum,
		GLuint* indices, int indicesNum,
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);

	//Assimp를 이용한 메쉬 로드
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
