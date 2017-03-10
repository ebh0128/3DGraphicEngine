
#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"

#include "DirLight.h"
#include "Geometry.h"
#include "Scene.h"
#include "Object.h"
#include "MyFrameBuffer.h"



DirLight::DirLight()
{

}
DirLight::DirLight(Node* parent, SceneGL* Scene) :Node(parent, Scene)
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
		{ 0 , 1 , 2 },
		{ 2 , 1 , 3 }
	};

	Mesh* QuadMesh = new Mesh((GLfloat*)Vertices, sizeof(Vertices) / sizeof(GLfloat),
		(GLuint*)Indices, sizeof(Indices) / sizeof(GLuint), nullptr);
	AddMesh(QuadMesh);

	pShader = new MyShader("./Shader/Deferred_DirLight.vert", "./Shader/Deferred_DirLight.frag");
	Diffuse = glm::vec3(0.7, 0.5, 0.3);
	Ambient = Specular = glm::vec3(0.3, 0.3, 0.3);
	
	pDefDirLitPass = new MyShader("./Shader/Deferred_DirLight.vert", "./Shader/Deferred_DirLight.frag");
	m_pDepthCopy = new MyShader("./Shader/DepthCopy.vert", "./Shader/DepthCopy.frag");
}
void DirLight::SetPos(glm::vec4 Lightpos)
{
	vPos = Lightpos;
}
void  DirLight::SetDiffuse(glm::vec3 dif)
{
	Diffuse = dif;
}
void  DirLight::SetSpec(glm::vec3 dif)
{
	Specular = dif;
}
void DirLight::Update(GLfloat dtime)
{

	Node::Update(dtime);
}

// 노드 메소드 그대로 사용 >> 디버그를 위해 일단 재정의
void DirLight::Render()
{
	
	Node::Render();
}
void DirLight::ShaderParamInit()
{
	pShader->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));

	//MVP 필요없음
	glm::mat4 VP = pScene->GetVPMatrix();
	pShader->SetUniformMatrix4fv("M", glm::value_ptr(TransformMat));
	pShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));


}

void DirLight::RenderDirLitPass()
{
	if (!pDefDirLitPass)  return;
	pDefDirLitPass->ApplyShader();

	for (GLuint i = 0; i<meshes.size(); i++)
	{
		// 변환 행렬 쉐이더 전송
		DirLitPassInit();
		meshes[i]->Render(1);
		
	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->RenderPointLitPass();
	}
}

void DirLight::CopyDepthPass()
{
	if (!m_pDepthCopy)  return;
	m_pDepthCopy->ApplyShader();

	for (GLuint i = 0; i<meshes.size(); i++)
	{
		// 변환 행렬 쉐이더 전송
		glm::vec2 ScreenSize = glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		pDefDirLitPass->SetUniform2fv("gScreenSize", glm::value_ptr(ScreenSize));

		pDefDirLitPass->SetUniform1i("gDepthTexture", DeferredRenderBuffers::TEXTURE_TYPE_TEXCOORD);


		meshes[i]->Render(1);

	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->RenderPointLitPass();
	}
}


void DirLight::DirLitPassInit()
{

	pDefDirLitPass->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));

	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 MVP = VP*TransformMat;
	pDefDirLitPass->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	pDefDirLitPass->SetUniformMatrix4fv("M", glm::value_ptr(TransformMat));
	pDefDirLitPass->SetUniformMatrix4fv("VP", glm::value_ptr(VP));

	glm::vec2 ScreenSize = glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	pDefDirLitPass->SetUniform2fv("gScreenSize", glm::value_ptr(ScreenSize));

	glm::vec4 CameraPos = pScene->GetCurrentCamPos();
	pDefDirLitPass->SetUniform4fv("gEyeWorldPos", glm::value_ptr(CameraPos));

	//Light 정보 보내기
	pDefDirLitPass->SetUniform4fv("gDirLight.LPos", glm::value_ptr(vPos));
	
	glm::vec4 paramDiff = glm::vec4(Diffuse, 1);
	glm::vec4 paramAmbi = glm::vec4(Ambient, 1);
	glm::vec4 paramSpec = glm::vec4(Specular, 1);

	pDefDirLitPass->SetUniform4fv("gDirLight.LDiff", glm::value_ptr(paramDiff));
	pDefDirLitPass->SetUniform4fv("gDirLight.LAmbi", glm::value_ptr(paramAmbi));
	pDefDirLitPass->SetUniform4fv("gDirLight.LSpec", glm::value_ptr(paramSpec));
	

	//gBuffer 텍스쳐 보내기
	pDefDirLitPass->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	pDefDirLitPass->SetUniform1i("gColorMap", DeferredRenderBuffers::TEXTURE_TYPE_DIFFUSE);
	pDefDirLitPass->SetUniform1i("gNormalMap", DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);

}
