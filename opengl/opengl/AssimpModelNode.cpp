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
	pShader = new MyShader();
	pShader->build_program_from_files("AssimpModel.vert", "AssimpModel.frag");
//	pShader->build_program_from_files("./Shader/Deferred_GeoPass.vert", "./Shader/Deferred_GeoPass.frag");

	pDefGeoPass = new MyShader();
	pDefGeoPass->build_program_from_files("./Shader/Deferred_GeoPass.vert", "./Shader/Deferred_GeoPass.frag");
	
	int strSize = sizeof(PaddingLight);

	m_pShaderShadow = new MyShader("./Shader/Shadow_InstanceObj.vert", "./Shader/Shadow_InstanceObj.frag");


	AddUBO(nullptr, strSize*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0 ,pShader);
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

	pShader = new MyShader();
		pShader->build_program_from_files("AssimpModel.vert", "AssimpModel.frag");
//	pShader->build_program_from_files("./Shader/Deferred_GeoPass.vert", "./Shader/Deferred_GeoPass.frag");
		 
	pDefGeoPass = new MyShader();
	pDefGeoPass->build_program_from_files("./Shader/Deferred_GeoPass.vert", "./Shader/Deferred_GeoPass.frag");


	m_pShaderShadow = new MyShader("./Shader/Shadow_InstanceObj.vert", "./Shader/Shadow_InstanceObj.frag");

	int strSize = sizeof(PaddingLight);
	AddUBO(nullptr, strSize*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0 , pShader);

	//�� ����
	
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
void AssimpObject::ShaderParamInit()
{

	//Dir Light ���� ������

	DirLight* pDirLit = pScene->GetDirectionalLight();
	glm::vec4 DirLightPos = pDirLit->GetPos();
	//glm::vec4 DirLightPos = glm::vec4(0,-15,0,1);
	pShader->SetUniform4fv("gDirLight.LPos", glm::value_ptr(DirLightPos));

	glm::vec4 paramDiff = glm::vec4(pDirLit->GetDif(), 1);
	glm::vec4 paramAmbi = glm::vec4(pDirLit->GetAmb(), 1);
	glm::vec4 paramSpec = glm::vec4(pDirLit->GetSpec(), 1);


	pShader->SetUniform4fv("gDirLight.LDiff", glm::value_ptr(paramDiff));
	pShader->SetUniform4fv("gDirLight.LAmbi", glm::value_ptr(paramAmbi));
	pShader->SetUniform4fv("gDirLight.LSpec", glm::value_ptr(paramSpec));
	pShader->SetUniform1i("TextureMain" , MainTextUnit);
	
	glm::vec4 CameraPos = pScene->GetCurrentCamPos();
	pShader->SetUniform4fv("gEyeWorldPos", glm::value_ptr(CameraPos));


	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 M;

	//if (mParent == nullptr) M = TransformMat;
	//else  M = TransformMat*mParent->GetModelMat();

	M = TransformMat;

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;
	pShader->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
	pShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	pShader->SetUniformMatrix4fv("V", glm::value_ptr(V));
	pShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	pShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	pShader->SetUniform1i("IsTextured", IsTextured ? 1 : 0);
	// �� ���� UiformBlock ���̴� ���� 
	LightList* DataforShader = pScene->GetLightSrouceArray();
	GLuint Size = DataforShader->Count * sizeof(PaddingLight);
	//meshes[i]->UpdateUBO(DataforShader, Size+ sizeof(GLuint), 0);
	UpdateUBO(DataforShader, sizeof(GLuint), 0);

	// std140 stride 16
	UpdateUBO(DataforShader, Size, 12);

}
void AssimpObject::GeoPassInit()
{

	//ShaderParamInit();
	
	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 M;

	//if (mParent == nullptr) M = TransformMat;
	//else  M = TransformMat*mParent->GetModelMat();

	M = TransformMat;

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;
	pDefGeoPass->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
	pDefGeoPass->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	pDefGeoPass->SetUniformMatrix4fv("V", glm::value_ptr(V));
	pDefGeoPass->SetUniformMatrix4fv("M", glm::value_ptr(M));
	pDefGeoPass->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	pDefGeoPass->SetUniform1i("IsTextured", IsTextured ? 1 : 0);
	pDefGeoPass->SetUniform1i("TextureMain", MainTextUnit);

}
void AssimpObject::ShadowPassInit()
{
	//�� ���� ��ȯ ���
	glm::mat4 LightSpaceMat = pScene->GetDirectionalLight()->GetLightVPMat();
	
	glm::mat4 M;
	//if (mParent == nullptr) M = TransformMat;
	//else  M = TransformMat*mParent->GetModelMat();

	M = TransformMat;

	m_pShaderShadow->SetUniformMatrix4fv("M", glm::value_ptr(M));
	m_pShaderShadow->SetUniformMatrix4fv("lightSpaceMat", glm::value_ptr(LightSpaceMat));

}

void AssimpObject::RenderGeoPass()
{
	Object::RenderGeoPass();
}
void AssimpObject::RenderShadowPass()
{
	Object::RenderShadowPass();
}