#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Scene.h"
#include "Object.h"
#include "AssimpModelNode.h"
#include "DroneCamera.h"
#include "SpotLight.h"
#include "Drone.h"



Drone::Drone(Node* Root, Object* Parent, SceneGL* Sce) : Object(Root,Parent ,Sce)
{
	

	MoveSpeed = 15;
	RotSpeed = 90;
	TransSet* WorldTransform = new TransSet();
	WorldTransform->vPos = glm::vec4(0, 50, 0, 1);
	WorldTransform->vRot = glm::vec3(0, 0, 0);
	WorldTransform->vScale = glm::vec3(10, 10, 10);
	AddInstance(WorldTransform);

	vFront = glm::vec3(0, 0, 1);
	vRight = glm::vec3(1, 0, 0);
	vUp = glm::vec3(0, 1, 0);


	pDroneCamera = new DroneCamera(glm::vec3(0,40,0),glm ::vec3(0,0,0),glm::vec3(0,0,1));
	Sce->AddCam(pDroneCamera);

	pSpotLight = new SpotLight(glm::vec3(0, 40, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)
								,glm::vec4(1,1,0,0) ,45, 0.2);
	Sce->SetSpotLight(pSpotLight);

}
void Drone::Update(GLfloat dtime)
{
	Object::Update(dtime);
}
void Drone::Render()
{
	Object::Render();
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
	InstanceList[0]->vRot.y += glm::radians(Angle);
	//월드 회전
	pDroneCamera->RotatebyWorldY( Angle);
	pSpotLight->RotatebyWorldY(Angle);
}

void Drone::MoveFront(float dist)
{
	InstanceList[0]->vPos += glm::vec4(vFront*dist,0);
	pDroneCamera->MovePosition(vFront, dist);
	pSpotLight->MovePosition(vFront, dist);
}