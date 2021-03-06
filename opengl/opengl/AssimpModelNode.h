#pragma once

class Object;

class AssimpObject : public Object
{
protected:
	bool IsRootNode;
	bool IsTextured;

	GLuint MainTextUnit;
public:

	AssimpObject();

	//자식 노드생성
	AssimpObject(Object* parent, SceneGL* Scene);
	
	//루트 노드 생성(Assimp Read)
	AssimpObject(Object* parent, SceneGL* Scene ,std::string FilePath, std::string FileName);
	
	virtual void InitModel(std::string FilePath, std::string FileName);
	virtual void ShaderInit();
	void SetNoTexture();
	void SetScale(glm::vec3 scale);

	GLuint GetMainTexUint() { return MainTextUnit; }
	bool GetIsTextured() { return IsTextured; }
	virtual void Update(GLfloat dtime);
	virtual void Render();

	virtual void ShaderParamInit(MyShader* ManagedShader = nullptr);
	
};