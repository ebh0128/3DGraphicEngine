#pragma once

class DroneCamera;

class Drone : public Object
{
protected:
	glm::vec3 vUp;
	glm::vec3 vRight;
	glm::vec3 vFront;

	float MoveSpeed;
	float RotSpeed;

	DroneCamera* pDroneCamera;
	SpotLight* pSpotLight;
public :
	Drone(Node* Root, Object* Parent, SceneGL* Sce);
	void Update(GLfloat dtime);
	void Render();

	void DroneMoveCallBackFunc(unsigned char key, GLfloat TickTime);
	void LightMoveCallBackFunc(unsigned char key, GLfloat TickTime);
	void CameraMoveCallBackFunc(unsigned char key, GLfloat TickTime);

	void RotatebyUp(float Angle);
	void MoveFront(float dist);
};