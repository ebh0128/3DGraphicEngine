#pragma once
#include "CommonHeader.h"
#include "Camera.h"

 

#include "SkyBox.h"

#include "DroneCamera.h"
#include "SpotLight.h"
#include "MyFrameBuffer.h"

#include "LightSystem.h"

#include "IOBuffer.h"
#include "Scene.h"
#include "DirLight.h"

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
	m_pShadowBuffer = new IOBuffer();
	m_pShadowBuffer->Init(1024, 1024, true, GL_NONE);

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
	m_pShadowBuffer = new IOBuffer();
	m_pShadowBuffer->Init(1024, 1024, true, GL_NONE);

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
		ShaderLightInfoList.Lights[i].Pos[3] = 1.f;
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
void SceneGL::SetSkyBox(SkyBox* Skybox)
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

void SceneGL::ShadowMapPass()
{
	glViewport(0,0,1024 , 1024);
	m_pShadowBuffer->BindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	//지오메트리에서 플랜드 필요없음
	glDisable(GL_BLEND);

	Root->RenderShadowPass();

	glViewport(0, 0, 1024, 768);
}


void SceneGL::DeferredRender(DeferredRenderBuffers* gBuffer, IOBuffer *SSAOBuffer , IOBuffer *BlurBuffer)
{
	//프레임버퍼 바인딩 후 초기화

	//if (pSkyBox != nullptr)pSkyBox->Render();

	//RenderGEODepth(gBuffer);
	ShadowMapPass();


	gBuffer->StartDeferredRender();
	glClearColor(0, 0, 0, 0);
	//glClearColor(1, 1, 1, 1);


	RenderGeoPass(gBuffer);

	SSAOPass(gBuffer, SSAOBuffer);

	BlurPass(BlurBuffer, SSAOBuffer);

	gBuffer->BindForGeomPass();
	
	//Dir 패스 시작
	
	/////////스탠실 만들기

	glEnable(GL_STENCIL_TEST);

	RenderStencilPass(gBuffer);
	
	/////////Light 패스 시작
	RenderPointLitPass(gBuffer);
	
	glDisable(GL_STENCIL_TEST);

	RenderDirLitPass(gBuffer , BlurBuffer);
	
	//Forward Rendering//////////////////////////
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	if (pSkyBox != nullptr) pSkyBox->RenderDepthRead();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	
	//m_pPointLightSys->Render();
	///////////////////////////////////////////
	//현재 출력 버퍼 HDR임
	//결과 출력
	RenderFinalPass(gBuffer);



	//Forward Render 시작
	
}
void SceneGL::RenderGEODepth(DeferredRenderBuffers* gBuffer)
{

	gBuffer->BindForGeoDepth();

	//깊이 버퍼는 지오메트리 에서만
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	//지오메트리에서 플랜드 필요없음
	glDisable(GL_BLEND);

	Root->RenderGeoPass();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

}

//디퍼드 랜더링 Geometry pass
void SceneGL::RenderGeoPass(DeferredRenderBuffers* gBuffer)
{
	gBuffer->BindForGeomPass();

	//깊이 버퍼는 지오메트리 에서만
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	//지오메트리에서 플랜드 필요없음
	glDisable(GL_BLEND);

	//Skybox Diffuse만 넣음
	
	Root->RenderGeoPass();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

}

void SceneGL::RenderStencilPass(DeferredRenderBuffers* gBuffer)
{


	//ColorBuffer 끄기
	gBuffer->BindForStencilPass();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_ALWAYS, 0, 0);
	
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	m_pPointLightSys->RenderStencilPass();
}


void  SceneGL::RenderPointLitPass(DeferredRenderBuffers* gBuffer)
{
	//빛의 결과는 순수하게 더해져야됨

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	gBuffer->BindForLightPass();
	glClear(GL_COLOR_BUFFER_BIT);

	m_pPointLightSys->RenderPointLitPass();

	glCullFace(GL_BACK);
	glDisable(GL_BLEND);

}
void  SceneGL::RenderDirLitPass(DeferredRenderBuffers* gBuffer, IOBuffer* BlurBuffer)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	//대략 잡은거
	BlurBuffer->BindForReading(GL_TEXTURE7);
	m_pShadowBuffer->BindForReading(GL_TEXTURE8);

	gBuffer->BindForLightPass();
	
	m_pDirLight->RenderDirLitPass();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	
}

void SceneGL::RenderFinalPass(DeferredRenderBuffers* gBuffer)
{
	GLsizei W = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei H = glutGet(GLUT_WINDOW_HEIGHT);

	gBuffer->BindForFinalPass();
	//그냥 그리기
	glBlitFramebuffer(0, 0, W, H, 0, 0, W, H, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	
	//HDR + reinhard tone mapping
	//m_pDirLight->HDRPass();
	
	//이 다음부터 Forward 해야되므로 초기화
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

}

void SceneGL::SSAOPass(DeferredRenderBuffers* gBuffer, IOBuffer* SSAOBuffer)
{
	gBuffer->BindForReading(DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	gBuffer->BindForReading(DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);

	SSAOBuffer->BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT);

	m_pDirLight->SSAOPass();

}
void SceneGL::BlurPass(IOBuffer* BlurBuffer, IOBuffer* SSAOBuffer)
{
	//컬러 어차피 1개임
	SSAOBuffer->BindForReading(GL_TEXTURE0);
	
	
	BlurBuffer->BindForWriting();

	glClear(GL_COLOR_BUFFER_BIT);

	m_pDirLight->BlurPass();
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