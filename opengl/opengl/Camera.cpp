#include"CommonHeader.h"
#include "Camera.h"

Camera::Camera()
{}
Camera::Camera(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up)
{

	CamID = -1;
	vPosition = Pos;
	vLookat = Lookat;
	vFront = glm::normalize(vLookat - vPosition);
	vUp = Up;

	fovy = glm::radians(60.f);
	ratio = glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT);
	fnear = 0.1f;
	ffar = 1000.0f;
	SpeedScale = 30.f;
	RotateSpeed = 30.f;
	Mode = FREE_CAMERA;
}


void Camera::SetPos(glm::vec3 Pos)
{
	vPosition = Pos;
	vFront = glm::normalize(vLookat - vPosition);
}
//Lookat을
void Camera::SetLookat(glm::vec3 Lookat)
{
	vLookat = Lookat;
	vFront = glm::normalize(vLookat - vPosition);
}

glm::vec3 Camera::GetPos()
{
	return vPosition;
}
glm::vec3 Camera::GetLookat()
{
	return vLookat;
}


void  Camera::ChangeModeCallBack()
{
	if (Mode & FREE_CAMERA)  FreeCameraInit();
	else RevolutionCameraInit();
}
void  Camera::FreeCameraInit()
{

}
void  Camera::RevolutionCameraInit()
{
	SetLookat(glm::vec3(0, 0, 0));
}

void Camera::SetMode(CameraMode mode)
{
	Mode = mode;
	ChangeModeCallBack();
}
CameraMode Camera::GetCameraMode()
{
	return Mode;
}
void Camera::CameraRotate(GLfloat xAngle, GLfloat yAngle, GLfloat zAngle)
{
	if (Mode > FREE_CAMERA) return;
	//업벡터 재계산을 위해
	glm::vec3 Right = glm::cross(vFront, vUp);

	vFront = glm::rotateX(vFront, glm::radians(xAngle*RotateSpeed));
	vFront = glm::rotateY(vFront, glm::radians(yAngle*RotateSpeed));
	vFront = glm::normalize(vFront);
	//프론트 벡터가 변화 햇으므로 응시지점도 바뀜
	SetLookat( vPosition + vFront);

	//프론트 벡터 변경시 업벡터가 뒤집힐수 있음
	vUp = glm::cross(Right, vFront);

	//뒤집히는거 이외에 기울어지는 효과는 무시(관찰 카메라 이므로 뒤집히면 불편)
	//if (vUp.y > 0) vUp = glm::vec3(0.f, 1.f, 0.f);
	//else vUp = glm::vec3(0.f, -1.f, 0.f);

}
void  Camera::CameraRevolution(GLfloat yAngle)
{
	//pos
	glm::vec4 TempEye = glm::vec4(vPosition,1);
	
	//이동 성분이 포함된 회전이므로 위치변환됨 
	TempEye = glm::rotateY(TempEye, yAngle);
	//Front 재계산이 필요하므로
	SetPos(TempEye);

}
//카메라 행렬 가져오기
glm::mat4 Camera::GetViewMat()
{

	return glm::lookAt(vPosition, vPosition+vFront, vUp);
}


//원근 투영
glm::mat4 Camera::GetPerspectiveLense()
{
	return glm::perspective(fovy, ratio, fnear, ffar);
	//return glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f , fnear ,ffar);
}

void Camera::SetPerspectiveLense(GLfloat _fovy, GLfloat _ratio, GLfloat _near, GLfloat _far)
{
	fovy  = _fovy;
	ratio = _ratio;
	fnear = _near;
	ffar = _far;
}

void Camera::RatioUpdate()
{
	ratio = glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT);
}

void Camera::KeyBoardCallBackFunc(unsigned char key, int x, int y, GLfloat TickTime)
{
	// 공통 이동
	switch (key)
	{
	case 'w':
		MoveForword(TickTime*SpeedScale);
		break;
	case 's':
		MoveForword(TickTime*(-SpeedScale));
		break;
	case 'a':
		MoveRight(TickTime*(-SpeedScale));
		break;
	case 'd':
		MoveRight(TickTime*SpeedScale);
		break;
	case 'q':
		MoveUp(TickTime * SpeedScale);
		break;
	case 'e':
		MoveUp(TickTime * (-SpeedScale));
		break;

	}
	if (Mode&FREE_CAMERA) FreeCameraControl(key, x, y, TickTime);
	else OriginRotateControl(key, x, y, TickTime);

}

void Camera::FreeCameraControl(unsigned char key, int x, int y, GLfloat TickTime)
{
	//Free Camera 움직임의 경우 여기에 추가
}
void Camera::OriginRotateControl(unsigned char key, int x, int y, GLfloat TickTime)
{
	//원점기준 회전 카메라의 경우 여기에
	switch (key)
	{
	case 'z':
		CameraRevolution(glm::radians(RotateSpeed)*TickTime);
		break;
	case  'c':
		CameraRevolution(glm::radians(RotateSpeed)*(-TickTime));
		break;
	}
}

void Camera::MoveForword(float val)
{
	//if (glm::distance(vPosition, vLookat) < 0.2f) return;

	vPosition = vPosition + vFront*val;
	//Look
	if(Mode & FREE_CAMERA)
		vLookat = vLookat + vFront*val;
}
void Camera::MoveUp(float val)
{
	SetPos(vPosition + vUp*val);
	if (Mode & FREE_CAMERA)
		SetLookat(vLookat + vUp*val);
}
void Camera::MoveRight(float val)
{
	glm::vec3 Right = glm::cross(vFront,vUp);
	Right = glm::normalize(Right);
	
	SetPos(vPosition + Right*val);
	if (Mode & FREE_CAMERA)
		SetLookat(vLookat + Right*val);
}

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////camera manager
/////////////////////////////////////////////////////////////////////////////////////////


CameraManager::CameraManager()
{}
void CameraManager::ChangeCamera(int index)
{
	CurrentCamIndex = index;
}
int CameraManager::AddCamera(Camera* pcam)
{
	int index = counter;
	CameraList.push_back(pcam);
	if (counter == 0) ChangeCamera(0);
	counter++;

	return index;
}


glm::mat4 CameraManager::GetViewMat()
{
	return CameraList[CurrentCamIndex]->GetViewMat();
}
glm::vec4 CameraManager::GetCurrentCamPos()
{
	return glm::vec4 (CameraList[CurrentCamIndex]->GetPos() , 1);
}

//원근 투영
glm::mat4 CameraManager::GetPerspectiveLense()
{
	return CameraList[CurrentCamIndex]->GetPerspectiveLense();
}
Camera* CameraManager::CreateCamera(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up)
{
	Camera* newcam = new Camera(Pos, Lookat, Up);
	AddCamera(newcam);
	return newcam;
}

void CameraManager::RatioUpdate()
{
	CameraList[CurrentCamIndex]->RatioUpdate();
}