#include "CommonHeader.h"
#include "SkinningModel.h"
#include "SkinnedObject.h"




SkinnedObject::SkinnedObject()
{

}

//자식 노드생성
SkinnedObject::SkinnedObject(Object* parent, SceneGL* Scene)
{

}

//루트 노드 생성(Assimp Read)
SkinnedObject::SkinnedObject(Object* parent, SceneGL* Scene, std::string FilePath, std::string FileName) :Object(parent , Scene )
{
	AnimationTimeCounter = 0;
	m_MainTextureUnitNum = 5;
	ShaderInit();
	InitModel(FilePath , FileName);
	vRot = glm::vec3(30, 0, 0);
	vPos = glm::vec4(0, 0, 0,1);

}
void SkinnedObject::InitModel(std::string FilePath, std::string FileName)
{
	m_pModel = new SkinningModel();
	m_pModel->CreateModelFromFile(FilePath , FileName);
	m_pSkinningModel = (SkinningModel*)m_pModel;

	
}

void SkinnedObject::ShaderInit()
{
	m_pShaderManager->CreateShader(this, "./Shader/Deferred_Skinning_GeoPass.vert", "./Shader/Deferred_Skinning_GeoPass.frag");
}
void SkinnedObject::Update(GLfloat dtime)
{
	AnimationTimeCounter += dtime;
	Object::Update(dtime);
}

void SkinnedObject::GetBoneMatrix(std::vector<glm::mat4>& GetVector)
{
	m_pSkinningModel->BoneTransform(AnimationTimeCounter, GetVector);
//	m_pSkinningModel->BoneTransform(1, GetVector);

}

