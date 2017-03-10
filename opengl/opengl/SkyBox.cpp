#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Scene.h"
#include "Texture.h"
#include "ProgramManager.h"
#include "SkyBox.h"
#include "DirLight.h"

SkyBox::SkyBox()
{

}
SkyBox::SkyBox(Node* parent, SceneGL* Scene) :Node(parent , Scene)
{
	//��ǻ� 2d �̹Ƿ� 2d �λ��
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
	pDefGeoPass = new MyShader("./Shader/Deferred_SkyBox.vert", "./Shader/Deferred_SkyBox.frag");
	Sampler* pSampler = new Sampler();
	CubeMapTexturePathInfo* pCubeMapPath = new CubeMapTexturePathInfo();

	
	pCubeMapPath->Path[POS_X] = "./Texture/right.jpg"; pCubeMapPath->Path[NEG_X] = "./Texture/left.jpg";
	pCubeMapPath->Path[POS_Y] = "./Texture/top.jpg"; pCubeMapPath->Path[NEG_Y] = "./Texture/bottom.jpg";
	pCubeMapPath->Path[POS_Z] = "./Texture/back.jpg"; pCubeMapPath->Path[NEG_Z] = "./Texture/front.jpg";

	//pSampler->CreateCubeMapTexture(pCubeMapPath, ProgramManager::GetInstance()->GetChannelID(), glGetUniformLocation(pShader->GetShaderProgram(),"SkyBoxTexture"));
	pSampler->CreateCubeMapTexture(pCubeMapPath, 15, glGetUniformLocation(pShader->GetShaderProgram(), "SkyBoxTexture"));

	SkyBoxMesh->AddSampler(pSampler);
	Diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1);
	Diffuse = glm::vec4(1, 1, 1, 1);

}
// ������ �����

void SkyBox::SetDiffuse(glm::vec4 Col)
{
	Diffuse = Col;
}
void SkyBox::Update(GLfloat dtime)
{

}
void SkyBox::Render()
{
	//���� �׽�Ʈ ����ϸ� �ȵ�
	glDisable(GL_DEPTH_TEST);
	if (pShader) pShader->ApplyShader();

	glm::mat4 V = pScene->GetVMatrix();
	//ī�޶� �̵��� ����
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
	//�������� �״�� ������
	for (GLuint i = 0; i<meshes.size(); i++)
	{
		meshes[i]->Render();
	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->Render();
	}
	glEnable(GL_DEPTH_TEST);
	//���� �ʱ�ȭ
	//glClear(GL_DEPTH_BUFFER_BIT);
}

void SkyBox::RenderGeoPass()
{
	//���� �׽�Ʈ ����ϸ� �ȵ�
	glDisable(GL_DEPTH_TEST);
	if (pDefGeoPass) pDefGeoPass->ApplyShader();

	glm::mat4 V = pScene->GetVMatrix();
	//ī�޶� �̵��� ����
	V[3][0] = 0; V[3][1] = 0; V[3][2] = 0;
	glm::mat4 P = pScene->GetPMatrix();
	glm::mat4 VP = P*V;
	glm::mat4 invV = glm::inverse(V);
	glm::mat4 invP = glm::inverse(P);
	glm::mat4 InversVP = glm::inverse(VP);
	//glm::mat4 InversVP = invP*invV;
	//InversVP = InversVP / InversVP[3][3];
	pDefGeoPass->SetUniformMatrix4fv("InversVP", glm::value_ptr(InversVP));

	glm::vec4 LitDir = pScene->GetDirectionalLight()->GetPos();
	LitDir = -LitDir;
	pDefGeoPass->SetUniform4fv("inversLightVector", glm::value_ptr(LitDir));
	pDefGeoPass->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));
	//�������� �״�� ������
	for (GLuint i = 0; i<meshes.size(); i++)
	{
		meshes[i]->Render();
	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->Render();
	}
	glEnable(GL_DEPTH_TEST);
	//���� �ʱ�ȭ
	glClear(GL_DEPTH_BUFFER_BIT);

}
void SkyBox::GeoPassInit()
{

}