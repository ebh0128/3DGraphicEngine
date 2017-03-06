#pragma once

class DroneCamera : public Camera
{
protected:
	glm::vec3 vRight;
public:
	DroneCamera();
	DroneCamera(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up);

	void MovePosition(glm::vec3 Dir , float dist);
	void RotatebyUp(float angle);
	void RotatebyRiht(float angle);
	void RotatebyFront(float angle);

	void RotatebyWorldY(float angle);
};