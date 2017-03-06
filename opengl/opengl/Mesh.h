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
// 씬에서 그려지는 오브젝트(BaseClass)
// 아직 텍스쳐 코드 관련 없음
// Assimp 모델과 호환 아직 고려하지 않았으나 최대한 어울릴수 있게 디자인
class Mesh
{

private:
	GLuint vao;
	GLuint ebo;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint instance_vbo;
	
	
	//여러개의 텍스쳐 코드 들어올수 있음
	GLuint vbo_texcoord[TEXCOORD_MAX];
	GLuint TexCoordCnt;

	Material* material;

	//정점 데이터 보관 (나중에 사용할일 없으면 지워서 최적화)

	//Primitive Count(오브젝트 로드시 자동계산)
	GLsizei PrimCount;
	GLsizei VerticesCount;

	std::vector<Sampler*> pMeshSamplerList;
	
	bool IsHaveNormal;
	bool IsHaveTexcoord;

private:
	//빛 계산하기때문에 노말 없으면 안됨(즉석에서 계산 하기)
	//왠만하면 노말 넘겨주자 (뺄셈 2번 + 외적 + 노말라이즈)
	GLfloat* NormalGenerate(GLfloat* vertices, GLuint* indices);

public:
	// 객체 이름
	char* Name;
	
	enum VBO_BUF_NAME { VBO_POSITION = 0 , VBO_NORMAL, NUM_VBOS};

	//default pos = 0 , norm = 1
	const GLuint attrib_Pos = 0;
	const GLuint attrib_Normal = 1;
	
	//시작 Attrib번호 즉 이후로는 attrib_Texcoord +1~+n
	const GLuint attrib_Texcoord =2;

	//인스턴싱용 5~
	const GLuint attrib_MVPMat = 5;

public:
	Mesh();
	Mesh(GLfloat* vertices , int VertexNum , 
		GLuint* indices, int indicesNum, 
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);
	
	Mesh(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh);
	~Mesh();

	//텍스쳐 코드는 여러개가 존재 가능하므로
	//어트리뷰트 확장시 이용할것
	void AddTexcoordAttibute(void* texcoords , int TexcoordNum);
	void SetMaterial(Material* pmat);
	Material* GetMaterial();
	
	void AddSampler(Sampler* pNewSamp);
	Sampler* CreateSampler(const char* ImagePath, GLuint Channel, GLint ShaderLoc , GLint DefShaderLoc = -500);

	//프로그램 내부에서 만든 메쉬 로드
	virtual void ObjectLoad(GLfloat* vertices, int VertexNum,
		GLuint* indices, int indicesNum,
		GLfloat* normals,
		GLfloat* texcoords = nullptr, int texcoordsNum = 0);

	//Assimp를 이용한 메쉬 로드
	virtual void ObjectLoad(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh);

	void MakeInstancingBuffer();
	void Render();
	void Render(glm::mat4 * MVPmats, unsigned int InstanceNum);

};


class SceneGL;
class Object;
//////////
///////// 여기부터 노드 클래스
class Node
{
protected:
	//어떤 오브젝트인가
	Object* pObj;
	// 현재 있는 씬
	SceneGL* pScene;
	// 여러 노드로 구성된 오브젝트 ()
	Node* Parent;
	std::vector<Node*> Children;
	std::vector<Mesh*> meshes;
	

	// 단일 & 여러 노드 오브젝트 공용
	//(자식노드일 경우 부모변환들이 계속 중첩)

	//모델공간
	glm::mat4 TransformMat;


	MyShader* pShader;

	MyShader* pDefGeoPass;
	MyShader* pDefLitPass;
	
	GLuint ubo;
	GLint UbSize;

public:
	char* Name;

	//모델을 이용하더라도 노드별로 Transform 가능하면 편함
	glm::vec4 vPos;
	glm::vec3 vRot;
	glm::vec3 vScale;

	Node();
	Node(Node* _parent, SceneGL* scene);
	~Node();

	//보통 1노드에 1메쉬쓰기때문에 필요(index 지정 안하면 0번메쉬)
	void SetMeshMaterial(Material* mat, int Index = 0);

	//오브젝트마다 재정의 해서 쓰기
	virtual void Update(GLfloat dtime);

	//재질적용방식이나 쉐이더 특이한거 적용시 재정의 해야됨
	virtual void Render();

	//디퍼드 랜더링
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


