#pragma once


enum CameraMode
{
	FREE_CAMERA = 0x0001,
	ORIGIN_ROTATE_CAMERA = 0x0002
};

//���� ī�޶� ��üȭ
class Camera
{
protected:

	int CamID;
	//��ġ, �ٶ󺸰��ִ���� ,Up����
	glm::vec3 vPosition;
	glm::vec3 vLookat;

	glm::vec3 vFront;
	glm::vec3 vUp;

	//���� ���� ����(�ۿ��� �ص� ������ ī�޶� ���� �������� ����)
	GLfloat fovy;
	GLfloat ratio;
	GLfloat fnear;
	GLfloat ffar;

	GLfloat SpeedScale;
	GLfloat RotateSpeed;

	// ����Ʈ�� Free Cam
	CameraMode Mode;
private:
	void FreeCameraControl(unsigned char key, int x, int y, GLfloat TickTime);

	//LookAt�� �������� ����
	// w s: ��
	//
	void OriginRotateControl(unsigned char key, int x, int y, GLfloat TickTime);
	
	//��� ��ȯ�� ī�޶� param ����
	void ChangeModeCallBack();

	//����ν�� �Ұ� ���� �����
	void FreeCameraInit();
	
	//������ �ٶ󺸰� �ʱ�ȭ
	void RevolutionCameraInit();
public:
	Camera();
	Camera(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up);
	
	void SetPos(glm::vec3 Pos);
	void SetLookat(glm::vec3 Lookat);
	void SetPerspectiveLense(GLfloat _fovy, GLfloat _ratio, GLfloat _near, GLfloat _far);
	
	//��尡 �ٲ�� �ʱ�ȭ
	void SetMode(CameraMode mode);
	CameraMode GetCameraMode();

	glm::vec3 GetPos();
	glm::vec3 GetLookat();

	//���� ���ͷ� �̵�
	void MoveForword(float val);
	//���� ���ͷ� �̵�
	void MoveRight(float val);
	//������ �������� �̵�
	void MoveUp(float val);
	void KeyBoardCallBackFunc(unsigned char key, int x, int y, GLfloat TickTime);
	
	//ī�޶��� ����Ʈ ���͸� Rotate
	//���� ī�޶� �϶��� ����
	void CameraRotate(GLfloat xAngle, GLfloat yAngle, GLfloat zAngle);

	//ī�޶� ��ġ�� Rotate (�� ������ �߽����� �� ����)
	void CameraRevolution(GLfloat yAngle);

	//ī�޶� ��� ��������
	glm::mat4 GetViewMat();

	//���� ����
	glm::mat4 GetPerspectiveLense();
	//���� ����
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

	//���� ����
	glm::mat4 GetPerspectiveLense();

	void RatioUpdate();
};