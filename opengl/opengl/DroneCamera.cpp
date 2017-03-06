#include"CommonHeader.h"
#include "Camera.h"
#include "DroneCamera.h"


DroneCamera::DroneCamera()
{}
DroneCamera::DroneCamera(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up) :Camera(Pos , Lookat , Up)
{
	vRight = glm::cross(vFront, vUp);
}

//Lookat도 같이 움직
void DroneCamera::MovePosition(glm::vec3 Dir, float dist)
{
	vPosition += Dir*dist;
	vLookat += Dir*dist;
}
void DroneCamera::RotatebyUp( float angle)
{
	vFront = glm::rotate(vFront, glm::radians(angle), vUp);
	vRight = glm::rotate(vRight, glm::radians(angle), vUp);
	
}
void DroneCamera::RotatebyRiht(float angle)
{
	vFront = glm::rotate(vFront, glm::radians(angle), vRight);
	vUp = glm::rotate(vUp, glm::radians(angle), vRight);
}
void DroneCamera::RotatebyFront(float angle)
{
	vRight = glm::rotate(vRight, glm::radians(angle), vFront);
	vUp = glm::rotate(vUp, glm::radians(angle), vFront);
}

void DroneCamera::RotatebyWorldY(float angle)
{
	vFront = glm::rotate(vFront, glm::radians(angle), glm::vec3(0,1,0));
	vRight = glm::rotate(vRight, glm::radians(angle), glm::vec3(0, 1, 0));
	vUp = glm::rotate(vUp, glm::radians(angle), glm::vec3(0, 1, 0));
}