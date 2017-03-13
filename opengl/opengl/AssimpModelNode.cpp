#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"

#include "Scene.h"
#include "AssimpModelNode.h"
#include "Object.h";
#include "DirLight.h"
#include "Texture.h"
AssimpModelNode::AssimpModelNode()
{
	IsTextured = true;
}
AssimpModelNode::AssimpModelNode(Node* parent, SceneGL* Scene) :Node(parent ,Scene)
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
AssimpModelNode::AssimpModelNode(Node* parent, SceneGL* Scene, std::string FilePath, std::string FileName):Node(parent, Scene)
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

	InitAssimpNode(AssimpScene->mRootNode, AssimpScene, FilePath);
	IsRootNode = true;
	IsTextured = true;

}

void AssimpModelNode::SetNoTexture()
{
	IsTextured = false;

	for (GLuint i = 0; i<Children.size(); i++)
	{
		((AssimpModelNode*)Children[i])->SetNoTexture();
	}
}
void AssimpModelNode::InitAssimpNode(aiNode* pNode, const aiScene* AssimpScene, std::string FilePath)
{
	
//	memcpy(&TransformMat, &(pNode->mTransformation.a1), sizeof(float) * 16);
	for (int i = 0; i < AssimpScene->mNumMeshes ; i++)
	{
		const aiMesh* AssimpMesh = AssimpScene->mMeshes[i];
		
		MeshEntry* NewMesh = new MeshEntry(AssimpScene, AssimpMesh);
		//텍스쳐 찾기
		aiMaterial* aiMat = AssimpScene->mMaterials[AssimpMesh->mMaterialIndex];
		aiString TexturePath;
		//텍스쳐의 상대경로를 알아냄 (Object 의 경로로부터의)
		aiReturn ret = aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &TexturePath);
		if (ret == AI_SUCCESS)
		{
			char* TPath = TexturePath.data;
			char* TextureName = strrchr(TPath, '/');
			if(TextureName == nullptr)  TextureName = strrchr(TPath, '\\');
			std::string sDir = FilePath + TextureName;
			//Sampler* MainSampler = new Sampler(sDir.c_str(), 7, glGetUniformLocation(pShader->GetShaderProgram(), "TextureMain"), glGetUniformLocation(pDefGeoPass->GetShaderProgram(), "TextureMain"));
			//NewMesh->AddSampler(MainSampler);
			Texture* MainTextue = new Texture(sDir.c_str(), MainTextUnit);
			NewMesh->AddTexture(MainTextue);
		}
		AddMesh(NewMesh);
	}
	/*
	for (int j = 0; j < pNode->mNumChildren ; j++)
	{
		AssimpModelNode* pChild = new AssimpModelNode(this, pScene);
		pChild->InitAssimpNode(pNode->mChildren[j], AssimpScene, FilePath);
	
	}
	*/
}
void AssimpModelNode::SetScale(glm::vec3 scale)
{
	vScale = scale;
}
void AssimpModelNode::Update(GLfloat dtime)
{
	
	Node::Update(dtime);

}
void AssimpModelNode::Render()
{
	Node::Render();
	
}
void AssimpModelNode::ShaderParamInit()
{

	//Dir Light 정보 보내기

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

	if (Parent == nullptr) M = TransformMat;
	else  M = TransformMat*Parent->GetModelMat();

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;
	pShader->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
	pShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	pShader->SetUniformMatrix4fv("V", glm::value_ptr(V));
	pShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	pShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	pShader->SetUniform1i("IsTextured", IsTextured ? 1 : 0);
	// 빛 정보 UiformBlock 쉐이더 전송 
	LightList* DataforShader = pScene->GetLightSrouceArray();
	GLuint Size = DataforShader->Count * sizeof(PaddingLight);
	//meshes[i]->UpdateUBO(DataforShader, Size+ sizeof(GLuint), 0);
	UpdateUBO(DataforShader, sizeof(GLuint), 0);

	// std140 stride 16
	UpdateUBO(DataforShader, Size, 12);

}
void AssimpModelNode::GeoPassInit()
{

	//ShaderParamInit();
	
	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 M;

	if (Parent == nullptr) M = TransformMat;
	else  M = TransformMat*Parent->GetModelMat();

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
void AssimpModelNode::ShadowPassInit()
{
	//빛 공간 변환 행렬
	glm::mat4 LightSpaceMat = pScene->GetDirectionalLight()->GetLightVPMat();
	
	glm::mat4 M;
	if (Parent == nullptr) M = TransformMat;
	else  M = TransformMat*Parent->GetModelMat();

	m_pShaderShadow->SetUniformMatrix4fv("M", glm::value_ptr(M));
	m_pShaderShadow->SetUniformMatrix4fv("lightSpaceMat", glm::value_ptr(LightSpaceMat));

}