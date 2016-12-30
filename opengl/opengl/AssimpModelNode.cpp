#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Scene.h"
#include "AssimpModelNode.h"
#include "Object.h";
AssimpModelNode::AssimpModelNode()
{
	IsTextured = true;
}
AssimpModelNode::AssimpModelNode(Node* parent, SceneGL* Scene) :Node(parent ,Scene)
{
	pShader = new MyShader();
	pShader->build_program_from_files("AssimpModel.vert", "AssimpModel.frag");
	AddUBO(nullptr, 16 * sizeof(GLfloat)*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0);
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

		pShader = new MyShader();
	pShader->build_program_from_files("AssimpModel.vert", "AssimpModel.frag");
	
	AddUBO(nullptr, 16 * sizeof(GLfloat)*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0);

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
	
	memcpy(&TransformMat, &(pNode->mTransformation.a1), sizeof(float) * 16);
	for (int i = 0; i < pNode->mNumMeshes ; i++)
	{
		const aiMesh* AssimpMesh = AssimpScene->mMeshes[pNode->mMeshes[i]];
		
		Mesh* NewMesh = new Mesh(AssimpScene, AssimpMesh);
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
			Sampler* MainSampler = new Sampler(sDir.c_str(), 7, glGetUniformLocation(pShader->GetShaderProgram(), "TextureMain"));
			NewMesh->AddSampler(MainSampler);
		}
		AddMesh(NewMesh);
	}
	for (int j = 0; j < pNode->mNumChildren ; j++)
	{
		AssimpModelNode* pChild = new AssimpModelNode(this, pScene);
		pChild->InitAssimpNode(pNode->mChildren[j], AssimpScene, FilePath);
	
	}
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
	if (pShader) pShader->ApplyShader();
	pScene->ApplySpotLight(pShader);
	if (IsRootNode)
	{
		int a = 0;
	}
	for (GLuint i = 0; i<meshes.size(); i++)
	{

		//메쉬별 쉐이더 재질 적용 
		Material* p = meshes[i]->GetMaterial();

		pShader->SetUniform3fv("material.diffuse", glm::value_ptr(p->diffuse));
		pShader->SetUniform3fv("material.amdient", glm::value_ptr(p->ambient));
		pShader->SetUniform3fv("material.specular", glm::value_ptr(p->specular));
		pShader->SetUniform1f("material.shininess", p->shininess);

		// 변환 행렬 쉐이더 전송
		glm::mat4 V = pScene->GetVMatrix();
		glm::mat4 VP = pScene->GetVPMatrix();
		glm::mat4 M;
	
		 M = TransformMat*Parent->GetModelMat();

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
		GLuint Size = DataforShader->Count * sizeof(GLfloat) * 16;
		//meshes[i]->UpdateUBO(DataforShader, Size+ sizeof(GLuint), 0);
		UpdateUBO(DataforShader, sizeof(GLuint), 0);

		// std140 stride 16
		UpdateUBO(DataforShader, Size, 12);
		if(pObj->GetInstanceNum() == 0) meshes[i]->Render();
		else meshes[i]->Render(pObj->GetInstanceMatrixData(), pObj->GetInstanceNum());
	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->Render();
	}

}