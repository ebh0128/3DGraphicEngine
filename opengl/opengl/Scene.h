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

//낮밤 변화시간
#define DAY_CHANGE_TIMEMAX 30
class Object;
class DroneCamera;
class SpotLight;

class SceneGL
{
protected:
	GLuint id;
	Object* Root;
	// 멀티 카메라 구현시 vector로 변경 고려

	CameraManager* pCamManager;
	//Camera* pCamera;

	//스카이박스는 따로 등록
	Node* pSkyBox;
	// 빛을 씬에서 관리(우선 정적 개수를 목표로)
	//사이즈가 작고 한번 생성되면 삭제 안함 -> 소멸시 다시 하늘로 이동
	int LightCnt;
	//구버젼
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
	
	//구버젼
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
	//디퍼드 랜더링 Geometry pass
	void RenderGeoPass();
	void RenderLitPass(DeferredRenderBuffers* gBuffer);
};