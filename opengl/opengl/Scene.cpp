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
	m_pShaderManager = new ShaderManager();
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

	m_pShaderManager = new ShaderManager();

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

		//만약 부모가 있다면 변환해서 줘야됨
	//	glm::mat4 LightTransform = InstanceL->GetMatWithoutScale();
		//glm::vec4 Result = LightTransform*glm::vec4(SourcePos, 1);
		//SourcePos = glm::vec3(Result);

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

	//Root->Update(dTime);
	m_pUpdateRoot->Update(dTime);
}

void SceneGL::SetUpdateRoot(ObjectInstance* ins)
{
	m_pUpdateRoot = ins;
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

void SceneGL::SetShaderManager(ShaderManager* ShManager)
{
	m_pShaderManager = ShManager;
}
ShaderManager* SceneGL::GetShaderManager()
{
	return m_pShaderManager;
}