
#include "CommonHeader.h"
#include "Camera.h"
#include "IOBuffer.h"


#include "Geometry.h"
#include "Scene.h"
#include "MyFrameBuffer.h"

#include "DirLight.h"
#include "LightSystem.h"


DirLight::DirLight()
{

}
DirLight::DirLight(Object* parent, SceneGL* Scene) :Object(parent, Scene)
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

	MeshEntry* QuadMesh = new MeshEntry((GLfloat*)Vertices, sizeof(Vertices) / sizeof(GLfloat),
		(GLuint*)Indices, sizeof(Indices) / sizeof(GLuint), nullptr);
	
	m_pModel = new Model();
	m_pModel->AddMesh(QuadMesh);
	

	pShader = new MyShader("./Shader/Deferred_DirLight.vert", "./Shader/Deferred_DirLight.frag");
	Diffuse = glm::vec3(0.7, 0.5, 0.3);
	//Diffuse = glm::vec3(70, 50, 30);

	Ambient = Specular = glm::vec3(0.6, 0.6, 0.6);
	
	pDefDirLitPass = new MyShader("./Shader/Deferred_DirLight.vert", "./Shader/Deferred_DirLight.frag");
	m_pShaderSSAO = new MyShader("./Shader/SSAO.vert", "./Shader/SSAO.frag");
	m_pShaderBlur = new MyShader("./Shader/Blur.vert", "./Shader/Blur.frag");
	m_pShaderHDR = new MyShader("./Shader/HDR.vert", "./Shader/HDR.frag");
	m_pShaderShadow = new MyShader("./Shader/HDR.vert", "./Shader/HDR.frag");

	
	LightOrtho = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, 0.1f, 1000.f);
		LightView = glm::lookAt(
		glm::vec3(-150, 225, 225)
		, glm::vec3(0, 0, 0)
		, glm::vec3(0, 1, 0));
	InitKernel();
	
}

void  DirLight::InitKernel()
{
	std::uniform_real_distribution<float> randomFloat(0.0, 1.0);
	std::default_random_engine fGenerator;
	for (int i = 0; i < KERNEL_SIZE*3; i+=3)
	{
		float scale = (float)i / (float)(KERNEL_SIZE);
		Kernel[i] = 2.0f * randomFloat(fGenerator)- 1.0f;
		Kernel[i + 1] = 2.0f * randomFloat(fGenerator) - 1.0f;
		Kernel[i + 2] = 2.0f * randomFloat(fGenerator) - 1.0f;
		
		// 원점에 최대한 가깝게 분포시킨것(lerp 효과)
		Kernel[i] *= (0.1f + 0.9f * scale * scale);
		Kernel[i + 1] *= (0.1f + 0.9f * scale * scale);
		Kernel[i + 2] *= (0.1f + 0.9f * scale * scale);

	}

	std::vector<glm::vec3> ssaoNoise;
	for (GLuint i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloat(fGenerator)*2.0f - 1.0f,
			randomFloat(fGenerator)*2.0f - 1.0f,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &NosieTexture);
	glBindTexture(GL_TEXTURE_2D, NosieTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	glm::vec4 ScaleLightPos = vPos * -15.f;
	LightView = glm::lookAt(
		 glm::vec3(ScaleLightPos.x , ScaleLightPos.y , ScaleLightPos.z) 
		,glm::vec3(0,0,0)
		,glm::vec3(0,1,0));


	Object::Update(dtime);
}

// 노드 메소드 그대로 사용 >> 디버그를 위해 일단 재정의
void DirLight::Render()
{
	
	Object::Render();
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

	DirLitPassInit();
	
	if (GetInstanceNum() == 0) m_pModel->Render();
	else m_pModel->Render(GetInstanceMatrixData(), GetInstanceNum());

	/*
	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->RenderPointLitPass();
	}
	*/
}

void DirLight::SSAOPass()
{
	if (!m_pShaderSSAO)  return;
	m_pShaderSSAO->ApplyShader();

	
	glm::mat4 P = pScene->GetPMatrix();


	m_pShaderSSAO->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	m_pShaderSSAO->SetUniform1i("gNormalMap", DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);
		
	//NoiseTexture 전송
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, NosieTexture);
	m_pShaderSSAO->SetUniform1i("gNoiseTexture", 10);

	m_pShaderSSAO->SetUniformMatrix4fv("P", glm::value_ptr(P));
	m_pShaderSSAO->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	m_pShaderSSAO->SetUniform1f("gSampleRad", 0.3f);
	m_pShaderSSAO->SetUniform3fv("gKernel", (GLfloat*)Kernel, KERNEL_SIZE);

	if (GetInstanceNum() == 0) m_pModel->Render();
	else m_pModel->Render(GetInstanceMatrixData(), GetInstanceNum());

	
	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->RenderPointLitPass();
	}
}
void DirLight::BlurPass()
{
	if (!m_pShaderBlur)  return;
	m_pShaderBlur->ApplyShader();

		// 변환 행렬 쉐이더 전송
	m_pShaderBlur->SetUniform1i("gColorMap", 0);

	if (GetInstanceNum() == 0) m_pModel->Render();
	else m_pModel->Render(GetInstanceMatrixData(), GetInstanceNum());

	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->RenderPointLitPass();
	}
}

void DirLight::HDRPass()
{
	if (!m_pShaderHDR)  return;
	m_pShaderHDR->ApplyShader();

		// 변환 행렬 쉐이더 전송
	
//	glm::mat4 P = pScene->GetPMatrix();

	m_pShaderHDR->SetUniform1i("gFinalMap", 4);
	m_pShaderHDR->SetUniform1f("exposure", 0.01);
//	m_pShaderHDR->SetUniform1f("exposure", 1);
//	m_pShaderSSAO->SetUniformMatrix4fv("P", glm::value_ptr(P));
//	m_pShaderSSAO->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	
	if (GetInstanceNum() == 0) m_pModel->Render();
	else m_pModel->Render(GetInstanceMatrixData(), GetInstanceNum());

	
	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->RenderPointLitPass();
	}
}


void DirLight::DirLitPassInit()
{

	pDefDirLitPass->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));

	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 MVP = VP*TransformMat;
	glm::mat4 V = pScene->GetVMatrix();
	pDefDirLitPass->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	pDefDirLitPass->SetUniformMatrix4fv("M", glm::value_ptr(TransformMat));
	pDefDirLitPass->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	pDefDirLitPass->SetUniformMatrix4fv("V", glm::value_ptr(V));

	glm::mat4 InvV = glm::inverse(V);
	pDefDirLitPass->SetUniformMatrix4fv("InverseV", glm::value_ptr(InvV));
	

	glm::vec2 ScreenSize = glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	pDefDirLitPass->SetUniform2fv("gScreenSize", glm::value_ptr(ScreenSize));

	glm::vec4 CameraPos = pScene->GetCurrentCamPos();
	//
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
	pDefDirLitPass->SetUniform1i("AOMap", 7);

	pDefDirLitPass->SetUniform1i("ShadowMap", 8);

	glm::mat4 LightSpaceMat = pScene->GetDirectionalLight()->GetLightVPMat();

	pDefDirLitPass->SetUniformMatrix4fv("lightSpaceMat", glm::value_ptr(LightSpaceMat));


}

