#pragma once


class SceneGL;
class Node;
class ObjectInstance;
class MeshEntry;
class MyShader;
class Model;

//실제 사용시엔 프로토타입 패턴으로 만들어사용
class Object
{
protected:
	Node* pRoot;
	SceneGL* pScene;
	Object* mParent;
	 
	//메쉬 정보
	Model*m_pModel;

	std::vector<ObjectInstance*> InstanceList;

	//Render 할때 사용하는 인스턴스 행렬 버퍼
	std::vector<glm::mat4> MatrixList;

	//필요없을듯함 어차피 인스턴스별로 자식 정하지 않음?
	std::vector<Object*> ChildList;

	//모델공간
	glm::mat4 TransformMat;

	//이동 예정
	MyShader* pShader;
	MyShader* pDefGeoPass;
	//포인트 라이트
	MyShader* pDefPtLitPass;
	//디렉션 라이트
	MyShader* pDefDirLitPass;
	MyShader* m_pShaderShadow;

	//빛 정보를 넘겨주기위한 버퍼(여기있어야되는지 검토는 필요)
	GLuint ubo;
	GLint UbSize;

	//모델 공간
	glm::vec4 vPos;
	glm::vec3 vRot;
	glm::vec3 vScale;

	GLuint MainTextureUnit;

public:
	//루트 노드를 지정하고 생성
	Object();
	Object(Object* Parent , SceneGL* Sce);
	
//	Object(MeshEntry* BaseMesh, Object* Parent, SceneGL* Sce);
//	Object(MeshEntry* BaseMesh, Object* Parent, SceneGL* Sce);

	void AddInstance(ObjectInstance* TsetInfo);
	virtual void Update(GLfloat dtime);
	virtual void Render();

	virtual void RenderGeoPass();
	virtual void RenderShadowPass();

	virtual void RenderPointLitPass() {}
	virtual void RenderDirLitPass() {}


	virtual void ShaderParamInit();
	virtual void GeoPassInit();
	virtual void ShadowPassInit();

	virtual void DirLitPassInit() {}
	virtual void PointLitPassInit() {}

	void AddChild(Object* pChild);
	Node* GetRoot();
	int GetInstanceNum();
	glm::mat4* GetInstanceMatrixData();
	glm::mat4 GetModelMat();

	//빛 정보를 받아보기위한 유니폼 블록
	//빛 정보가 필요하면 써야됨
	void AddUBO(void* Data, GLuint Size, const char* BlockName, GLuint* Offset, MyShader* pshad);
	void UpdateUBO(void* Data, GLuint Size, GLuint Offset);

	void SetPosition(float x, float y, float z);
	glm::vec4 GetPos() { return vPos; }

};

