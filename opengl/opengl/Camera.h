#pragma once


enum CameraMode
{
	FREE_CAMERA = 0x0001,
	ORIGIN_ROTATE_CAMERA = 0x0002
};

//월드 카메라 객체화
class Camera
{
protected:

	int CamID;
	//위치, 바라보고있는장소 ,Up벡터
	glm::vec3 vPosition;
	glm::vec3 vLookat;

	glm::vec3 vFront;
	glm::vec3 vUp;

	//원근 투영 관련(밖에서 해도 되지만 카메라 렌즈 개념으로 생각)
	GLfloat fovy;
	GLfloat ratio;
	GLfloat fnear;
	GLfloat ffar;

	GLfloat SpeedScale;
	GLfloat RotateSpeed;

	// 디폴트는 Free Cam
	CameraMode Mode;
private:
	void FreeCameraControl(unsigned char key, int x, int y, GLfloat TickTime);

	//LookAt이 원점으로 고정
	// w s: 줌
	//
	void OriginRotateControl(unsigned char key, int x, int y, GLfloat TickTime);
	
	//모드 전환시 카메라 param 조절
	void ChangeModeCallBack();

	//현재로써는 할게 없어 비워둠
	void FreeCameraInit();
	
	//원점을 바라보게 초기화
	void RevolutionCameraInit();
public:
	Camera();
	Camera(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up);
	
	void SetPos(glm::vec3 Pos);
	void SetLookat(glm::vec3 Lookat);
	void SetPerspectiveLense(GLfloat _fovy, GLfloat _ratio, GLfloat _near, GLfloat _far);
	
	//모드가 바뀌면 초기화
	void SetMode(CameraMode mode);
	CameraMode GetCameraMode();

	glm::vec3 GetPos();
	glm::vec3 GetLookat();

	//정면 벡터로 이동
	void MoveForword(float val);
	//측면 벡터로 이동
	void MoveRight(float val);
	//업벡터 방향으로 이동
	void MoveUp(float val);
	void KeyBoardCallBackFunc(unsigned char key, int x, int y, GLfloat TickTime);
	
	//카메라의 프론트 벡터를 Rotate
	//프리 카메라 일때만 동작
	void CameraRotate(GLfloat xAngle, GLfloat yAngle, GLfloat zAngle);

	//카메라 위치를 Rotate (즉 원점을 중심으로 한 공전)
	void CameraRevolution(GLfloat yAngle);

	//카메라 행렬 가져오기
	glm::mat4 GetViewMat();

	//원근 투영
	glm::mat4 GetPerspectiveLense();
	//비율 재계산
	void RatioUpdate();
	
};

class CameraManager
{
protected:
	int counter;
	int CurrentCamIndex;
	std::vector<Camera*> CameraList;
public:
	CameraManager();

	Camera* CreateCamera(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up);
	void ChangeCamera(int index);
	int AddCamera(Camera* pcam);

	glm::mat4 GetViewMat();

	//원근 투영
	glm::mat4 GetPerspectiveLense();

	void RatioUpdate();
};