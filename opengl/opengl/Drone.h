#pragma once

class DroneCamera;

class Drone : public AssimpObject
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
	Drone(Object* Parent, SceneGL* Sce);
	Drone(Object* parent, SceneGL* Scene, std::string FilePath, std::string FileName);

	
	virtual void Render();

	virtual void RenderGeoPass();
	virtual void RenderShadowPass();


	virtual void ShaderParamInit();
	virtual void GeoPassInit();
	virtual void ShadowPassInit();

	void DroneMoveCallBackFunc(unsigned char key, GLfloat TickTime);
	void LightMoveCallBackFunc(unsigned char key, GLfloat TickTime);
	void CameraMoveCallBackFunc(unsigned char key, GLfloat TickTime);

	void RotatebyUp(float Angle);
	void MoveFront(float dist);
};