#include "CommonHeader.h"
#include "Camera.h"

#include "Scene.h"

#include "AssimpModelNode.h"
#include "DroneCamera.h"
#include "SpotLight.h"

#include "Drone.h"



Drone::Drone( Object* Parent, SceneGL* Sce) : AssimpObject(Parent ,Sce)
{
	

	MoveSpeed = 15;
	RotSpeed = 90;
	//모델공간 초기값
	//vPos = glm::vec4(0, 50, 0 ,1);

	ObjectInstance* NewInstance = new ObjectInstance(this);
	NewInstance->SetPos(glm::vec3(0, 50, 0));
//	NewInstance->SetRot(glm::vec3(0, 0, 0));
	NewInstance->SetScale(glm::vec3(1, 1, 1));
	AddInstance(NewInstance);

	vFront = glm::vec3(0, 0, 1);
	vRight = glm::vec3(1, 0, 0);
	vUp = glm::vec3(0, 1, 0);


	pDroneCamera = new DroneCamera(glm::vec3(0,40,0),glm ::vec3(0,0,0),glm::vec3(0,0,1));
	Sce->AddCam(pDroneCamera);

	pSpotLight = new SpotLight(glm::vec3(0, 40, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)
								,glm::vec4(1,1,0,0) ,45, 0.2);
	Sce->SetSpotLight(pSpotLight);

}

Drone::Drone(Object* parent, SceneGL* Scene, std::string FilePath, std::string FileName):AssimpObject(parent, Scene , FilePath, FileName)
{
	MoveSpeed = 15;
	RotSpeed = 90;

	

	vFront = glm::vec3(0, 0, 1);
	vRight = glm::vec3(1, 0, 0);
	vUp = glm::vec3(0, 1, 0);


	pDroneCamera = new DroneCamera(glm::vec3(0, 40, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
	Scene->AddCam(pDroneCamera);

	pSpotLight = new SpotLight(glm::vec3(0, 40, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)
		, glm::vec4(1, 1, 0, 0), 45, 0.2);
	Scene->SetSpotLight(pSpotLight);
	SetNoTexture();
}


void Drone::DroneMoveCallBackFunc(unsigned char key, GLfloat TickTime)
{
	
	switch (key)
	{
	case 'w':
		MoveFront(TickTime*MoveSpeed);
		break;
	case 's':
		MoveFront(TickTime*(-MoveSpeed));
		break;
	case 'a':
		RotatebyUp(TickTime*(-RotSpeed));
		break;
	case 'd':
		RotatebyUp(TickTime*RotSpeed);
		break;
	

	}

}

void Drone::LightMoveCallBackFunc(unsigned char key, GLfloat TickTime)
{
	switch (key)
	{
	case 'w':
		pSpotLight->RotatebyRiht(TickTime*(RotSpeed));
		break;
	case 's':
		pSpotLight->RotatebyRiht(TickTime*(-RotSpeed));
		break;
	case 'a':
		pSpotLight->RotatebyUp(TickTime*(-RotSpeed));
		break;
	case 'd':
		pSpotLight->RotatebyUp(TickTime*(RotSpeed));
		break;
	}
}
void Drone::CameraMoveCallBackFunc(unsigned char key, GLfloat TickTime)
{
	switch (key)
	{
	case 'w':
		pDroneCamera->RotatebyRiht(TickTime*(RotSpeed));
		break;
	case 's':
		pDroneCamera->RotatebyRiht(TickTime*(-RotSpeed));
		break;
	case 'a':
		pDroneCamera->RotatebyUp(TickTime*(-RotSpeed));
		break;
	case 'd':
		pDroneCamera->RotatebyUp(TickTime*(RotSpeed));
		break;
	}
}

//회전
void Drone::RotatebyUp(float Angle)
{
	vFront = glm::rotate(vFront, glm::radians(Angle), vUp);
	vRight = glm::rotate(vRight, glm::radians(Angle), vUp);
	//InstanceList[0]->vRot.y += glm::radians(Angle);
	glm::vec3 TempRot = glm::vec3(0,0,0);
	TempRot.y += glm::radians(Angle);
	InstanceList[0]->Rotation(TempRot);

	//월드 회전
	pDroneCamera->RotatebyWorldY( Angle);
	pSpotLight->RotatebyWorldY(Angle);
}

void Drone::MoveFront(float dist)
{
	InstanceList[0]->SetPos(InstanceList[0]->GetPos() + glm::vec3(vFront*dist));
	//InstanceList[0]->vPos += glm::vec4(vFront*dist,0);
	pDroneCamera->MovePosition(vFront, dist);
	pSpotLight->MovePosition(vFront, dist);
}

