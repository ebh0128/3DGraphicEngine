#pragma once

class Light;
class LightInstance;
class DeferredRenderBuffers;

struct PaddingLight
{
	GLfloat Pos[4];
	GLfloat Diffuse[4];
	GLfloat Ambient[4];
	GLfloat Specular[4];

};

struct LightList
{
	GLuint Count;
	PaddingLight Lights[LIGHT_MAX];
};

enum TimeForLight
{
	DAY,NIGHT 
};

//���� ��ȭ�ð�
#define DAY_CHANGE_TIMEMAX 30
class Object;
class DroneCamera;
class SpotLight;

class SceneGL
{
protected:
	GLuint id;
	Object* Root;
	// ��Ƽ ī�޶� ������ vector�� ���� ���

	CameraManager* pCamManager;
	//Camera* pCamera;

	//��ī�̹ڽ��� ���� ���
	Node* pSkyBox;
	// ���� ������ ����(�켱 ���� ������ ��ǥ��)
	//����� �۰� �ѹ� �����Ǹ� ���� ���� -> �Ҹ�� �ٽ� �ϴ÷� �̵�
	int LightCnt;
	//������
	Light* pLightList[LIGHT_MAX];
	LightInstance* pLightBuffer[LIGHT_MAX];

	Light* pDirectionalLight;
	
	SpotLight* pSpotLight;

	GLfloat TickTimerCounter;

	LightList ShaderLightInfoList;
	TimeForLight CurruntState;
public:

	const glm::vec3 NightDiffuse = glm::vec3(19 / 255, 12 / 255, 159 / 255);
	const glm::vec3 DayDiffuse = glm::vec3(0.7, 0.7, 0.7);

	SceneGL();
	SceneGL(Object* root, Camera* cam);
	~SceneGL();

	Camera* CreateCamera(glm::vec3 Pos, glm::vec3 Lookat, glm::vec3 Up);
	void ChangeCamera(int index);
	void RatioUpdate();


	Light* CreateLight(Node* parent, glm::vec4 Pos);
	Light* CreateLight(Node* parent, glm::vec4 Pos , glm::vec3 Diffuse);
	
	//������
	void AddLight(Light* plight);

	void AddLight(LightInstance* pins);

	void SetSpotLight(SpotLight* pSpot);
	void ApplySpotLight(MyShader* pshader);

	void AddCam(Camera* cam);
	void SetRoot(Object* root);
	void SetSkyBox(Node* Skybox);


	void SetDirectionalLight(Light* pLight);
	Light* GetDirectionalLight();

	void SmoothTimeChange(GLfloat dTime);

	glm::mat4 GetVPMatrix();
	glm::mat4 GetVMatrix();
	glm::mat4 GetPMatrix();

	LightList* GetLightSrouceArray();

	void ChangeTime(TimeForLight time);

	void Update(GLfloat dTime);
	void Render();

	void DeferredRender(DeferredRenderBuffers* gBuffer);
	//���۵� ������ Geometry pass
	void RenderGeoPass();
	void RenderLitPass(DeferredRenderBuffers* gBuffer);
};