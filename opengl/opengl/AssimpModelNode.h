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

	//�ڽ� ������
	AssimpObject(Object* parent, SceneGL* Scene);
	
	//��Ʈ ��� ����(Assimp Read)
	AssimpObject(Object* parent, SceneGL* Scene ,std::string FilePath, std::string FileName);
	
	void SetNoTexture();
	void SetScale(glm::vec3 scale);
	virtual void Update(GLfloat dtime);
	virtual void Render();

	virtual void RenderGeoPass();
	virtual void RenderShadowPass();


	virtual void ShaderParamInit(MyShader* ManagedShader = nullptr);
	virtual void GeoPassInit(MyShader* ManagedShader = nullptr);
	//void LitPassInit();
	virtual void ShadowPassInit(MyShader* ManagedShader = nullptr);

};