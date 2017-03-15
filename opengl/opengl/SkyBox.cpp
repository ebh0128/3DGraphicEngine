#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"

#include "Scene.h"
#include "Texture.h"
 
#include "SkyBox.h"
#include "DirLight.h"

SkyBox::SkyBox()
{

}
SkyBox::SkyBox(Object* parent, SceneGL* Scene) :Object(parent , Scene)
{
	//사실상 2d 이므로 2d 로사용
	float DepthMax =0.9999999f;
	Vec Vertices[] =
	{
		{ -1.00f , -1.00f , DepthMax },
		{ 1.00f , -1.00f , DepthMax },
		{ -1.00f , 1.00f , DepthMax },
		{ 1.00f , 1.00f , DepthMax },
	
		
	};
	InVec Indices[] = 
	{
		{0 , 1 , 2},
		{2 , 1 , 3}
	};

	MeshEntry* SkyBoxMesh = new MeshEntry((GLfloat*)Vertices, sizeof(Vertices) / sizeof(GLfloat),
					(GLuint*)Indices, sizeof(Indices) / sizeof(GLuint), nullptr);
	m_pModel = new Model();
	m_pModel->AddMesh(SkyBoxMesh);
	
	
//	pShader = new MyShader("SkyBox.vert","SkyBox.frag");
	ForwardShaderName = m_pShaderManager->CreateShader(this, "SkyBox.vert", "SkyBox.frag");
	//pDefGeoPass = new MyShader("./Shader/Deferred_SkyBox.vert", "./Shader/Deferred_SkyBox.frag");
	CubeMapTexturePathInfo* pCubeMapPath = new CubeMapTexturePathInfo();

	
	pCubeMapPath->Path[POS_X] = "./Texture/right.jpg"; pCubeMapPath->Path[NEG_X] = "./Texture/left.jpg";
	pCubeMapPath->Path[POS_Y] = "./Texture/top.jpg"; pCubeMapPath->Path[NEG_Y] = "./Texture/bottom.jpg";
	pCubeMapPath->Path[POS_Z] = "./Texture/back.jpg"; pCubeMapPath->Path[NEG_Z] = "./Texture/front.jpg";

	
	MainTextureUnit = 12;
	Texture* CubeTexture = new Texture();
	//CubeTexture->SetShaderValue(glGetUniformLocation(pShader->GetShaderProgram(), "SkyBoxTexture"));
	CubeTexture->CreateCubeMapTexture(pCubeMapPath, MainTextureUnit);
	SkyBoxMesh->AddTexture(CubeTexture);


	Diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1);
	Diffuse = glm::vec4(1, 1, 1, 1);

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
	MyShader* ThisShader = m_pShaderManager->ApplyShaderByName(ForwardShaderName);

	if (!ThisShader) return;

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
	ThisShader->SetUniformMatrix4fv("InversVP", glm::value_ptr(InversVP));
	ThisShader->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));
	//포지션은 그대로 보낼것
	
		m_pModel->Render();
	
	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->Render();
	}
	glEnable(GL_DEPTH_TEST);
	//깊이 초기화
	//glClear(GL_DEPTH_BUFFER_BIT);
}

void SkyBox::RenderDepthRead()
{
	MyShader* ThisShader = m_pShaderManager->ApplyShaderByName(ForwardShaderName);

	if (!ThisShader) return;


	GLint getDepth =123;
	glGetIntegerv(GL_DEPTH_WRITEMASK , &getDepth);
	glm::mat4 V = pScene->GetVMatrix();
	//카메라 이동은 무시
	V[3][0] = 0; V[3][1] = 0; V[3][2] = 0;
	glm::mat4 P = pScene->GetPMatrix();
	glm::mat4 VP = P*V;
	glm::mat4 invV = glm::inverse(V);
	glm::mat4 invP = glm::inverse(P);
	glm::mat4 InversVP = glm::inverse(VP);
	//glm::mat4 InversVP = invP*invV;
	//InversVP = InversVP / InversVP[3][3];
	ThisShader->SetUniformMatrix4fv("InversVP", glm::value_ptr(InversVP));

	glm::vec4 LightDiffuse = glm::vec4 (pScene->GetDirectionalLight()->GetDif(),1);
	ThisShader->SetUniform4fv("DiffuseCol", glm::value_ptr(LightDiffuse));
	ThisShader->SetUniform1i("SkyBoxTexture", MainTextureUnit);
	//포지션은 그대로 보낼것
	m_pModel->Render();

	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->Render();
	}
	
}
void SkyBox::GeoPassInit()
{

}