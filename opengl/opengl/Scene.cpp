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
#include "Scene.h"


SceneGL::SceneGL()
{
	Root = nullptr;
	//pCamera = nullptr;
	pCamManager = new CameraManager();
	LightCnt = 0;
	memset(pLightList, 0, sizeof(Light*)*LIGHT_MAX);
	pSkyBox = nullptr;
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
	memset(pLightList, 0, sizeof(Light*)*LIGHT_MAX);
	pSkyBox = nullptr;
	TickTimerCounter = 0;
	CurruntState = DAY;

}

//루트부터 순회하며 삭제
SceneGL::~SceneGL()
{

}

Light* SceneGL::CreateLight(Node* parent, glm::vec4 Pos)
{
	if (LightCnt > LIGHT_MAX) return nullptr;
	Light* NewLight = new Light(parent,this);
	NewLight->SetPos(Pos);
	AddLight(NewLight);

	return NewLight;
}
Light* SceneGL::CreateLight(Node* parent, glm::vec4 Pos , glm::vec3 Diffuse)
{
	if (LightCnt > LIGHT_MAX) return nullptr;
	Light* NewLight = new Light(parent, this);
	NewLight->SetPos(Pos);
	NewLight->SetDiffuse(Diffuse);
	AddLight(NewLight);

	return NewLight;
}
void SceneGL::AddLight(Light* plight)
{
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		if (pLightList[i] != NULL) continue;
		pLightList[i] = plight;
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
	for (i = 0; i < LIGHT_MAX; i++)
	{
		if (pLightList[i] == NULL) continue;
		
		memcpy(ShaderLightInfoList.Lights[i].Pos, glm::value_ptr(pLightList[i]->vPos), 4 * sizeof(GLfloat));
		memcpy(ShaderLightInfoList.Lights[i].Diffuse, glm::value_ptr(pLightList[i]->Diffuse), 3 * sizeof(GLfloat));
		memcpy(ShaderLightInfoList.Lights[i].Ambient, glm::value_ptr(pLightList[i]->Ambient), 3 * sizeof(GLfloat));
		memcpy(ShaderLightInfoList.Lights[i].Specular, glm::value_ptr(pLightList[i]->Specular), 3 * sizeof(GLfloat));

		//행렬로 쓰기위해 추가한 더미값
		ShaderLightInfoList.Lights[i].Diffuse[3] = 0.f; ShaderLightInfoList.Lights[i].Ambient[3] = 0.f; ShaderLightInfoList.Lights[i].Specular[3] = 0.f;

		//LightCnt++;
	}
	ShaderLightInfoList.Count = LightCnt;
	return &ShaderLightInfoList;
}
void SceneGL::SmoothTimeChange(GLfloat dTime)
{
	TickTimerCounter += dTime;
	/*
	if (TickTimerCounter > DAY_CHANGE_TIMEMAX)
	{
		TickTimerCounter = 0;
		//MaxState
		CurruntState = (TimeForLight)((CurruntState + 1) % 2);
	}
	float InterConst = TickTimerCounter / DAY_CHANGE_TIMEMAX;

	if (CurruntState == NIGHT)
	{
		glm::vec3 col = (1 - InterConst)*NightDiffuse + InterConst*DayDiffuse;
		GetDirectionalLight()->SetDiffuse(col);
		((SkyBox*)pSkyBox)->SetDiffuse(glm::vec4(col, 1.0f));
	}
	else
	{
		glm::vec3 col = (1 - InterConst)*DayDiffuse + InterConst*NightDiffuse;
		GetDirectionalLight()->SetDiffuse(col);
		((SkyBox*)pSkyBox)->SetDiffuse(glm::vec4(col, 1.0f));
	}
	*/
	
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
	SmoothTimeChange(dTime);
	Root->Update(dTime);
}
void SceneGL::SetDirectionalLight(Light* pLight)
{
	pDirectionalLight = pLight;
}
Light* SceneGL::GetDirectionalLight()
{
	return pDirectionalLight;
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderGeoPass();

	//원래 프레임버퍼 바인딩후 초기화
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//읽기위해 버퍼 바인딩
	gBuffer->BindForReading();
	RenderLitPass(gBuffer);
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
	
	//Test Code
	GLsizei W = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei H = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei HalfWidth = (GLsizei)( W / 2.0f);
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