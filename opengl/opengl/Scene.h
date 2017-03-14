#pragma once

class Object;
class Light;
class LightSystem;
class LightInstance;
class DirLight;
class DeferredRenderBuffers;
class IOBuffer;

struct PaddingLight
{
	GLfloat Pos[4];
	GLfloat Diffuse[4];
	GLfloat Ambient[4];
	GLfloat Specular[4];
	GLfloat Attnuation[4];
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

enum DeferredState
{
	DEFERRED, FORWARD
};
//낮밤 변화시간
#define DAY_CHANGE_TIMEMAX 30

class DroneCamera;
class SpotLight;
class SkyBox;


class SceneGL
{
protected:
	GLuint id;
	//오브젝트 트리 (업데이트시 계층따라감)
	Object* Root;
	ObjectInstance* m_pUpdateRoot;
	
	//Render용 메쉬 버퍼

	Object* TerrianMesh;
	std::vector<Object*> Meshes;

	CameraManager* pCamManager;
	//Camera* pCamera;

	//스카이박스는 따로 등록
	SkyBox* pSkyBox;

	IOBuffer* m_pShadowBuffer;

	DeferredState m_RenderState;
	//Deferred Lighting 에서 계층 탐색없이 바로 찾아가기위해 사용
	LightSystem* m_pPointLightSys;
	int LightCnt;
	//구버젼
	LightInstance* pLightBuffer[LIGHT_MAX];

	DirLight* m_pDirLight;
	
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

	void SetUpdateRoot(ObjectInstance* ins);
	void SetRoot(Object* root);
	void SetSkyBox(SkyBox* Skybox);

	void SetLightSystem(LightSystem* lsys) { m_pPointLightSys = lsys; }

	void SetDirectionalLight(DirLight* pLight);
	DirLight* GetDirectionalLight();

	glm::mat4 GetVPMatrix();
	glm::mat4 GetVMatrix();
	glm::mat4 GetPMatrix();

	LightList* GetLightSrouceArray();

	void ChangeTime(TimeForLight time);

	void Update(GLfloat dTime);
	void Render();

	void DeferredRender(DeferredRenderBuffers* gBuffer , IOBuffer *SSAOBuffer , IOBuffer *BlurBuffer);
	
protected:
	
	//디퍼드 랜더링 Geometry pass
	void RenderGeoPass(DeferredRenderBuffers* gBuffer);
	
	void RenderStencilPass(DeferredRenderBuffers* gBuffer);

	//Lightt Pass	
	void RenderPointLitPass(DeferredRenderBuffers* gBuffer);
	void RenderDirLitPass(DeferredRenderBuffers* gBuffe , IOBuffer* BlurBuffer);
	void RenderFinalPass(DeferredRenderBuffers* gBuffer);
	
	//디버그용 함수
	void DrawGBuffer(DeferredRenderBuffers* gBuffer);

	void RenderGEODepth(DeferredRenderBuffers* gBuffer);
	void SSAOPass(DeferredRenderBuffers* gBuffer, IOBuffer* SSAOBuffer);
	void BlurPass(IOBuffer* BlurBuffer, IOBuffer* SSAOBuffer);
	

	void ShadowMapPass();

//	glm::vec4 GetDriLightPos() { return m_pDirLight->GetPos(); }

};