#include "CommonHeader.h"
#include "Camera.h"


#include "Scene.h"

#include "AssimpModelNode.h"

#include "Texture.h"
#include "DirLight.h"
AssimpObject::AssimpObject()
{
	IsTextured = true;
}
AssimpObject::AssimpObject(Object* parent, SceneGL* Scene) :Object(parent ,Scene)
{
	ForwardShaderName = m_pShaderManager->CreateShader(this, "AssimpModel.vert", "AssimpModel.frag");
	GeoShaderName = m_pShaderManager->CreateShader(this, "./Shader/Deferred_GeoPass.vert", "./Shader/Deferred_GeoPass.frag");

	int strSize = sizeof(PaddingLight);

	ShadowShaderName = m_pShaderManager->CreateShader(this, "./Shader/Shadow_InstanceObj.vert", "./Shader/Shadow_InstanceObj.frag");
	

	AddUBO(nullptr, strSize*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0, m_pShaderManager->ApplyShaderByName(ForwardShaderName));
	IsRootNode = false;
	IsTextured = true;

}
AssimpObject::AssimpObject(Object* parent, SceneGL* Scene, std::string FilePath, std::string FileName):Object(parent, Scene)
{
	Assimp::Importer importer;
	std::string FullPath = FilePath + FileName;
	const aiScene* AssimpScene = importer.ReadFile(FullPath,
		aiProcess_Triangulate		|
		aiProcess_JoinIdenticalVertices	|
		aiProcess_GenSmoothNormals	);

	MainTextUnit = 5;

	
	ForwardShaderName = m_pShaderManager->CreateShader(this, "AssimpModel.vert", "AssimpModel.frag");
	ShadowShaderName = m_pShaderManager->CreateShader(this, "./Shader/Shadow_InstanceObj.vert", "./Shader/Shadow_InstanceObj.frag");
	GeoShaderName = m_pShaderManager->CreateShader(this, "./Shader/Deferred_GeoPass.vert", "./Shader/Deferred_GeoPass.frag");

	
	int strSize = sizeof(PaddingLight);
		AddUBO(nullptr, strSize*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0 , m_pShaderManager->ApplyShaderByName(ForwardShaderName));

	//모델 버젼
	
	m_pModel = new Model();
	m_pModel->CreateModelFromFile(FilePath, FileName);


	IsRootNode = true;
	IsTextured = true;

}

void AssimpObject::SetNoTexture()
{
	IsTextured = false;

}

void AssimpObject::SetScale(glm::vec3 scale)
{
	vScale = scale;
}
void AssimpObject::Update(GLfloat dtime)
{
	
	Object::Update(dtime);

}
void AssimpObject::Render()
{
	Object::Render();
	
}
void AssimpObject::ShaderParamInit(MyShader* ManagedShader)
{
	MyShader* ThisShader;
	if (ManagedShader == nullptr) return;
	else ThisShader = ManagedShader;
	//Dir Light 정보 보내기

	DirLight* pDirLit = pScene->GetDirectionalLight();
	glm::vec4 DirLightPos = pDirLit->GetPos();
	//glm::vec4 DirLightPos = glm::vec4(0,-15,0,1);
	ThisShader->SetUniform4fv("gDirLight.LPos", glm::value_ptr(DirLightPos));

	glm::vec4 paramDiff = glm::vec4(pDirLit->GetDif(), 1);
	glm::vec4 paramAmbi = glm::vec4(pDirLit->GetAmb(), 1);
	glm::vec4 paramSpec = glm::vec4(pDirLit->GetSpec(), 1);


	ThisShader->SetUniform4fv("gDirLight.LDiff", glm::value_ptr(paramDiff));
	ThisShader->SetUniform4fv("gDirLight.LAmbi", glm::value_ptr(paramAmbi));
	ThisShader->SetUniform4fv("gDirLight.LSpec", glm::value_ptr(paramSpec));
	ThisShader->SetUniform1i("TextureMain" , MainTextUnit);
	
	glm::vec4 CameraPos = pScene->GetCurrentCamPos();
	ThisShader->SetUniform4fv("gEyeWorldPos", glm::value_ptr(CameraPos));


	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 M;

	//if (mParent == nullptr) M = TransformMat;
	//else  M = TransformMat*mParent->GetModelMat();

	M = TransformMat;

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;
	ThisShader->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
	ThisShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	ThisShader->SetUniformMatrix4fv("V", glm::value_ptr(V));
	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	ThisShader->SetUniform1i("IsTextured", IsTextured ? 1 : 0);
	// 빛 정보 UiformBlock 쉐이더 전송 
	LightList* DataforShader = pScene->GetLightSrouceArray();
	GLuint Size = DataforShader->Count * sizeof(PaddingLight);
	//meshes[i]->UpdateUBO(DataforShader, Size+ sizeof(GLuint), 0);
	UpdateUBO(DataforShader, sizeof(GLuint), 0);

	// std140 stride 16
	UpdateUBO(DataforShader, Size, 12);

}
void AssimpObject::GeoPassInit(MyShader* ManagedShader)
{

	MyShader* ThisShader;
	if (ManagedShader == nullptr) return;
	else ThisShader = ManagedShader;
	
	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 M;

	//if (mParent == nullptr) M = TransformMat;
	//else  M = TransformMat*mParent->GetModelMat();

	M = TransformMat;

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;
	ThisShader->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
	ThisShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	ThisShader->SetUniformMatrix4fv("V", glm::value_ptr(V));
	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	ThisShader->SetUniform1i("IsTextured", IsTextured ? 1 : 0);
	ThisShader->SetUniform1i("TextureMain", MainTextUnit);

}
void AssimpObject::ShadowPassInit(MyShader* ManagedShader)
{
	MyShader* ThisShader;
	if (ManagedShader == nullptr) return;
	else ThisShader = ManagedShader;
	//빛 공간 변환 행렬
	glm::mat4 LightSpaceMat = pScene->GetDirectionalLight()->GetLightVPMat();
	
	glm::mat4 M;
	//if (mParent == nullptr) M = TransformMat;
	//else  M = TransformMat*mParent->GetModelMat();
	 
	M = TransformMat;

	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("lightSpaceMat", glm::value_ptr(LightSpaceMat));

}

void AssimpObject::RenderGeoPass()
{
	Object::RenderGeoPass();
}
void AssimpObject::RenderShadowPass()
{
	Object::RenderShadowPass();
}