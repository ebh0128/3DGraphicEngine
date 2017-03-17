#pragma once


class SkinningModel;
class SkinnedObject : public Object
{
protected:
	float AnimationTimeCounter;
	
	//단순히 포인터(매번 캐스팅해서 사용하지 않기위해 사용)
	//일반 모델을 캐스팅해서 사용해도 됨
	SkinningModel* m_pSkinningModel;

	GLuint m_MainTextureUnitNum;

public :
	SkinnedObject();

	//자식 노드생성
	SkinnedObject(Object* parent, SceneGL* Scene);

	//루트 노드 생성(Assimp Read)
	SkinnedObject(Object* parent, SceneGL* Scene, std::string FilePath, std::string FileName);
	void ShaderInit() override;

	void InitModel(std::string FilePath, std::string FileName);
	void GetBoneMatrix(std::vector<glm::mat4>& GetVector);
	virtual void Update(GLfloat dtime);
	GLuint GetMainTextureUnit() { return m_MainTextureUnitNum; }
};