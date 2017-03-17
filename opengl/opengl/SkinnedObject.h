#pragma once


class SkinningModel;
class SkinnedObject : public Object
{
protected:
	float AnimationTimeCounter;
	
	//�ܼ��� ������(�Ź� ĳ�����ؼ� ������� �ʱ����� ���)
	//�Ϲ� ���� ĳ�����ؼ� ����ص� ��
	SkinningModel* m_pSkinningModel;

	GLuint m_MainTextureUnitNum;

public :
	SkinnedObject();

	//�ڽ� ������
	SkinnedObject(Object* parent, SceneGL* Scene);

	//��Ʈ ��� ����(Assimp Read)
	SkinnedObject(Object* parent, SceneGL* Scene, std::string FilePath, std::string FileName);
	void ShaderInit() override;

	void InitModel(std::string FilePath, std::string FileName);
	void GetBoneMatrix(std::vector<glm::mat4>& GetVector);
	virtual void Update(GLfloat dtime);
	GLuint GetMainTextureUnit() { return m_MainTextureUnitNum; }
};