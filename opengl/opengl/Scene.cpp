#pragma once
#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "Sampler.h"
#include "SkyBox.h"
#include "Object.h"
#include "DroneCamera.h"
#include "SpotLight.h"
#include "MyFrameBuffer.h"
#include "ObjectInstance.h"
#include "LightSystem.h"
#include "DirLight.h"
#include "Scene.h"


SceneGL::SceneGL()
{
	Root = nullptr;
	//pCamera = nullptr;
	pCamManager = new CameraManager();
	LightCnt = 0;
	pSkyBox = nullptr;
	m_pPointLightSys = nullptr;
	m_pDirLight = nullptr;

	TickTimerCounter = 0;
	CurruntState = DAY;
}
SceneGL::SceneGL(Object* root, Camera* cam)
{
	Root = root;
	//pCamera = cam;
	pCamManager = new CameraManager();
	pCamManager->ChangeCamera(pCamManager->AddCamera(cam));

	LightCnt = 0;
	pSkyBox = nullptr;
	m_pPointLightSys = nullptr;
	m_pDirLight = nullptr;
	TickTimerCounter = 0;
	CurruntState = DAY;

}

//루트부터 순회하며 삭제
SceneGL::~SceneGL()
{

}

void SceneGL::AddLight(LightInstance* pins)
{
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		if (pLightBuffer[i] != NULL) continue;
		pLightBuffer[i] = pins;
		LightCnt++;
		break;
	}
}

glm::mat4 SceneGL::GetVPMatrix()
{
	//glm::mat4 View = pCamera->GetViewMat();
	//glm::mat4 Proj = pCamera->GetPerspectiveLense();


	glm::mat4 View = pCamManager->GetViewMat();
	glm::mat4 Proj = pCamManager->GetPerspectiveLense();
	glm::mat4 VP = Proj*View;

	return VP;
}
glm::mat4 SceneGL::GetVMatrix()
{
	//return pCamera->GetViewMat();
	return pCamManager->GetViewMat();
}
glm::mat4 SceneGL::GetPMatrix()
{
	//return pCamera->GetPerspectiveLense();
	return pCamManager->GetPerspectiveLense();
}


LightList* SceneGL::GetLightSrouceArray()
{
	int i = 0;
	//배열로 한번에 보내기 위해
	

	//int LightCnt = 0;
	for (i = 0; i < LightCnt; i++)
	{
		if (pLightBuffer[i] == NULL) continue;
		
		//memcpy(ShaderLightInfoList.Lights[i].Pos, glm::value_ptr(pLightList[i]->vPos), 4 * sizeof(GLfloat));
		//memcpy(ShaderLightInfoList.Lights[i].Diffuse, glm::value_ptr(pLightList[i]->Diffuse), 3 * sizeof(GLfloat));
		//memcpy(ShaderLightInfoList.Lights[i].Ambient, glm::value_ptr(pLightList[i]->Ambient), 3 * sizeof(GLfloat));
		//memcpy(ShaderLightInfoList.Lights[i].Specular, glm::value_ptr(pLightList[i]->Specular), 3 * sizeof(GLfloat));
		glm::vec3 SourcePos = pLightBuffer[i]->GetPos();
		glm::vec3 SourceDif = pLightBuffer[i]->GetDif();
		glm::vec3 SourceAmb = pLightBuffer[i]->GetAmbi();
		glm::vec3 SourceSpec = pLightBuffer[i]->GetSpec();

		memcpy(ShaderLightInfoList.Lights[i].Pos, glm::value_ptr(SourcePos), 3 * sizeof(GLfloat));
		memcpy(ShaderLightInfoList.Lights[i].Diffuse, glm::value_ptr(SourceDif), 3 * sizeof(GLfloat));
		memcpy(ShaderLightInfoList.Lights[i].Ambient, glm::value_ptr(SourceAmb), 3 * sizeof(GLfloat));
		memcpy(ShaderLightInfoList.Lights[i].Specular, glm::value_ptr(SourceSpec), 3 * sizeof(GLfloat));

		
		ShaderLightInfoList.Lights[i].Diffuse[3] = pLightBuffer[i]->DiffuseFactor; 
		ShaderLightInfoList.Lights[i].Ambient[3] = pLightBuffer[i]->AmbientFactor;
		ShaderLightInfoList.Lights[i].Specular[3] = pLightBuffer[i]->SpecularFactor;
		
		LitAttnu Atn = pLightBuffer[i]->GetAttnuation();
		ShaderLightInfoList.Lights[i].Attnuation[0] = Atn.Constant;
		ShaderLightInfoList.Lights[i].Attnuation[1] = Atn.Linear;
		ShaderLightInfoList.Lights[i].Attnuation[2] = Atn.exp;

		//행렬로 쓰기위해 추가한 더미값
		ShaderLightInfoList.Lights[i].Pos[3] = 0.f;
		ShaderLightInfoList.Lights[i].Attnuation[3] = 0.f;
		//LightCnt++;
	}
	ShaderLightInfoList.Count = LightCnt;
	return &ShaderLightInfoList;
}

glm::vec4 SceneGL::GetCurrentCamPos()
{
	return pCamManager->GetCurrentCamPos();
}
void SceneGL::AddCam(Camera* cam)
{
	//pCamera = cam;
	pCamManager->AddCamera(cam);
}
void SceneGL::SetRoot(Object* root)
{
	Root = root;
}
void SceneGL::SetSkyBox(Node* Skybox)
{
	pSkyBox = Skybox;
}
void SceneGL::Update(GLfloat dTime)
{
	if (m_pDirLight != nullptr) m_pDirLight->Update(dTime);
	Root->Update(dTime);
}
void SceneGL::SetDirectionalLight(DirLight* pLight)
{
	m_pDirLight = pLight;
}
DirLight* SceneGL::GetDirectionalLight()
{
	return m_pDirLight;
}

Camera* SceneGL::CreateCamera(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up)
{
	return pCamManager->CreateCamera(Pos, Lookat, Up);
}
void SceneGL::ChangeTime(TimeForLight time)
{
	CurruntState = time;
	switch (time)
	{

	case NIGHT:
		GetDirectionalLight()->SetDiffuse(NightDiffuse);
		((SkyBox*)pSkyBox)->SetDiffuse(glm::vec4(NightDiffuse,1.0f));
		break;
	case DAY:
		GetDirectionalLight()->SetDiffuse(DayDiffuse);
		((SkyBox*)pSkyBox)->SetDiffuse(glm::vec4(DayDiffuse, 1.0f));
		break;
	}
}
void SceneGL::Render()
{
	if (Root == nullptr)return;


	
	if(pSkyBox != nullptr)pSkyBox->Render();
	Root->Render();

	//디버그용 코드
	//if(m_pDirLight != nullptr) m_pDirLight->Render();
}

void  SceneGL::RatioUpdate()
{
	pCamManager->RatioUpdate();
}

void  SceneGL::ChangeCamera(int index)
{
	pCamManager->ChangeCamera(index);
}

void SceneGL::ApplySpotLight(MyShader* pshader)
{
	if (pSpotLight != nullptr) pSpotLight->TransferToShader(pshader);
}


void SceneGL::SetSpotLight(SpotLight* pSpot)
{
	pSpotLight = pSpot;
}

void SceneGL::DeferredRender(DeferredRenderBuffers* gBuffer)
{
	//프레임버퍼 바인딩 후 초기화
	gBuffer->BindForWriting();
	
	//깊이 버퍼는 지오메트리 에서만
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
	//지오메트리에서 플랜드 필요없음
	glDisable(GL_BLEND);

	RenderGeoPass();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	/////////Light 패스 시작

	//원래 프레임버퍼 바인딩후 초기화
	
	//읽기위해 버퍼 바인딩
	InitLightPass(gBuffer);
	RenderLitPass(gBuffer);
	
	//Dir 패스 시작
	//RenderDirLitPass(gBuffer);
	
	//디버그용 테스트 코드
	//gBuffer->BindForReading();
	//	DrawGBuffer(gBuffer);

}
//디퍼드 랜더링 Geometry pass
void SceneGL::RenderGeoPass()
{
//	Root->Render();
	
	Root->RenderGeoPass();
}
void SceneGL::RenderLitPass(DeferredRenderBuffers* gBuffer)
{
	//Root->RenderLitPass();
	RenderPointLitPass(gBuffer);
	RenderDirLitPass(gBuffer);

	
}
void SceneGL::InitLightPass(DeferredRenderBuffers* gBuffer)
{
	//빛의 결과는 순수하게 더해져야됨
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	gBuffer->BindForReading();
	glClear(GL_COLOR_BUFFER_BIT);
}

void  SceneGL::RenderPointLitPass(DeferredRenderBuffers* gBuffer)
{
	m_pPointLightSys->RenderPointLitPass();
}
void  SceneGL::RenderDirLitPass(DeferredRenderBuffers* gBuffer)
{
	m_pDirLight->RenderDirLitPass();
}

void SceneGL::DrawGBuffer(DeferredRenderBuffers* gBuffer)
{

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Test Code
	GLsizei W = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei H = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei HalfWidth = (GLsizei)(W / 2.0f);
	GLsizei HalfHeight = (GLsizei)(H / 2.0f);

	gBuffer->SetReadBuffer(DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(0, 0, W, H,
		0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	gBuffer->SetReadBuffer(DeferredRenderBuffers::TEXTURE_TYPE_DIFFUSE);
	glBlitFramebuffer(0, 0, W, H,
		0, HalfHeight, HalfWidth, H, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	gBuffer->SetReadBuffer(DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);
	glBlitFramebuffer(0, 0, W, H,
		HalfWidth, HalfHeight, W, H, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	gBuffer->SetReadBuffer(DeferredRenderBuffers::TEXTURE_TYPE_TEXCOORD);
	glBlitFramebuffer(0, 0, W, H,
		HalfWidth, 0, W, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}