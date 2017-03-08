#pragma once

class Light;
class LightSystem;
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


	
	//Deferred Lighting 에서 계층 탐색없이 바로 찾아가기위해 사용
	LightSystem* m_pPointLightSys;
	int LightCnt;
	//구버젼
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
	glm::vec4 GetCurrentCamPos();

	
	void AddLight(LightInstance* pins);

	void SetSpotLight(SpotLight* pSpot);
	void ApplySpotLight(MyShader* pshader);

	void AddCam(Camera* cam);

	void SetRoot(Object* root);
	void SetSkyBox(Node* Skybox);

	void SetLightSystem(LightSystem* lsys) { m_pPointLightSys = lsys; }

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
	
protected:
	
	//디퍼드 랜더링 Geometry pass
	void RenderGeoPass();
	
	//Lightt Pass
	void InitLightPass(DeferredRenderBuffers* gBuffer);
	void RenderLitPass(DeferredRenderBuffers* gBuffer);

	void RenderPointLitPass(DeferredRenderBuffers* gBuffer);
	void RenderDirLitPass(DeferredRenderBuffers* gBuffer);
	//디버그용 함수
	void DrawGBuffer(DeferredRenderBuffers* gBuffer);
};