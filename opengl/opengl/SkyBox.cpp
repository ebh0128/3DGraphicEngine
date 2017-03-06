#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Scene.h"
#include "Texture.h"
#include "ProgramManager.h"
#include "SkyBox.h"


SkyBox::SkyBox()
{

}
SkyBox::SkyBox(Node* parent, SceneGL* Scene) :Node(parent , Scene)
{
	//사실상 2d 이므로 2d 로사용
	Vec Vertices[] =
	{
		{ -1.00f , -1.00f , 0 },
		{ 1.00f , -1.00f , 0 },
		{ -1.00f , 1.00f , 0 },
		{ 1.00f , 1.00f , 0 },
	
		
	};
	InVec Indices[] = 
	{
		{0 , 1 , 2},
		{2 , 1 , 3}
	};

	Mesh* SkyBoxMesh = new Mesh((GLfloat*)Vertices, sizeof(Vertices) / sizeof(GLfloat),
					(GLuint*)Indices, sizeof(Indices) / sizeof(GLuint), nullptr);
	AddMesh(SkyBoxMesh);

	pShader = new MyShader("SkyBox.vert","SkyBox.frag");
	Sampler* pSampler = new Sampler();
	CubeMapTexturePathInfo* pCubeMapPath = new CubeMapTexturePathInfo();

	
	pCubeMapPath->Path[POS_X] = "./Texture/right.jpg"; pCubeMapPath->Path[NEG_X] = "./Texture/left.jpg";
	pCubeMapPath->Path[POS_Y] = "./Texture/top.jpg"; pCubeMapPath->Path[NEG_Y] = "./Texture/bottom.jpg";
	pCubeMapPath->Path[POS_Z] = "./Texture/back.jpg"; pCubeMapPath->Path[NEG_Z] = "./Texture/front.jpg";

	//pSampler->CreateCubeMapTexture(pCubeMapPath, ProgramManager::GetInstance()->GetChannelID(), glGetUniformLocation(pShader->GetShaderProgram(),"SkyBoxTexture"));
	pSampler->CreateCubeMapTexture(pCubeMapPath, 0, glGetUniformLocation(pShader->GetShaderProgram(), "SkyBoxTexture"));

	SkyBoxMesh->AddSampler(pSampler);
	Diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1);
}
// 완전히 덮어씌움

void SkyBox::SetDiffuse(glm::vec4 Col)
{
	Diffuse = Col;
}
void SkyBox::Update(GLfloat dtime)
{

}
void SkyBox::Render()
{
	//깊이 테스트 사용하면 안됨
	glDisable(GL_DEPTH_TEST);
	if (pShader) pShader->ApplyShader();

	glm::mat4 V = pScene->GetVMatrix();
	//카메라 이동은 무시
	V[3][0] = 0 ; V[3][1] = 0; V[3][2] = 0;
	glm::mat4 P = pScene->GetPMatrix();
	glm::mat4 VP =  P*V;
	glm::mat4 invV = glm::inverse(V);
	glm::mat4 invP = glm::inverse(P);
	glm::mat4 InversVP = glm::inverse(VP);
	//glm::mat4 InversVP = invP*invV;
	//InversVP = InversVP / InversVP[3][3];
	pShader->SetUniformMatrix4fv("InversVP", glm::value_ptr(InversVP));
	pShader->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));
	//포지션은 그대로 보낼것
	for (GLuint i = 0; i<meshes.size(); i++)
	{
		meshes[i]->Render();
	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->Render();
	}
	glEnable(GL_DEPTH_TEST);
	//깊이 초기화
	glClear(GL_DEPTH_BUFFER_BIT);
}