
///////////////////////////////////////////////////////////////////////
#include "CommonHeader.h"


#include "Camera.h"
#include "Timer.h"

#include "PatchedGround.h"
 
#include "Scene.h"
#include "SkyBox.h"
#include "MyFrameBuffer.h"
#include "AssimpModelNode.h"

#include "Drone.h"
#include "ProgramManager.h"
#include "Renderer.h"

#include "LightSystem.h"
#include "DirLight.h"

//60프레임 에서의 1프레임당 밀리초 16
#define TIME_PER_ONE_FRAME 10


ProgramManager* ProgramManager::m_pInstance = nullptr;

GLuint	vao;
GLuint	vbo[2], ebo;
GLuint	h_prog;
static GLuint	attrib_position = 0;
static GLuint	attrib_color = 7;
enum VBO_Bufs { VBO_POSITION = 0,  NUM_VBOS = 2 };

// 마우스 움직임 관련
enum GUIstate { STATE_NONE = 0, STATE_ROTATING, STATE_MOVING, STATE_NUM };
struct
{
	glm::ivec2  offset, position_mouse;
	GLuint      state;
} gui =
{
	glm::ivec2(0,0),
	glm::ivec2(0,0),
	STATE_NONE,
};

enum InputMode {CAM , DRONE , DRONE_CAM , DRONE_SPOT};

InputMode iMode;

MyShader* TestShader;
Camera* MyCamera;
Timer* TickTimer;

PatchedGround* GoodGround;
SceneGL* Scene;
GLuint CurrentPollygonMode;
SkyBox* pSkyBox;
AssimpObject* ModelTest;

Drone* againDrone;
Renderer* SceneRenderer;
LightSystem *LightSys;
DirLight* MainDirLight;


void init(void)
{
	iMode = CAM;
	//DevIL 초기화
	ilInit();
	ilutRenderer(ILUT_OPENGL);
	//씬 만들고
	Scene = new SceneGL();
	//카메라 생성
	MyCamera = new Camera(glm::vec3(0, 128, 128), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//	MyCamera = new Camera(glm::vec3(-150, 225, 225), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	//땅 생성
	
	GoodGround = new PatchedGround(nullptr, Scene, 102, 64.f);
	//GoodGround->Create(256, 256, 0.5f, 10, 10);
	GoodGround->Create(512, 512, 0.5f, 10, 10);

	//GroundObject = new Object(GoodGround,nullptr,Scene);
	
	ObjectInstance* GroundInstance = new ObjectInstance(GoodGround);
	GroundInstance->SetPos(glm::vec3(0, 0, 0));
//	GroundInstance->SetRot(glm::vec3(0, 0, 0));
	GroundInstance->SetScale(glm::vec3(1, 1, 1));

	GoodGround->AddInstance(GroundInstance);

	Scene->AddCam(MyCamera);
	Scene->SetRoot((Object*)GoodGround);
	Scene->SetUpdateRoot(GroundInstance);

	glClearColor(0.3, 0.3, 0.3, 1.0);
	
	CurrentPollygonMode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, CurrentPollygonMode);
	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);
	// Add as many OpenGL function calls as needed here...
	float ratio = glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT);
	
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);
	//인스턴스버젼 빛

	
	//빛 생 성!(랜덤하게)
	
	//Dir부터
	MainDirLight = new DirLight(nullptr, Scene);
	MainDirLight->SetPos(glm::vec4(10, -15, -15 ,1));
	MainDirLight->SetDiffuse(glm::vec3(0.8, 0.5, 0.4));
	//MainDirLight->SetDiffuse(glm::vec3(80, 50, 40));

	Scene->SetDirectionalLight(MainDirLight);

	againDrone = new Drone(GoodGround, Scene, "./Model/Monkey", "/monkey_high.obj");
	againDrone->SetPosition(0,50,0);
	againDrone->SetScale(glm::vec3(10, 10, 10));

	ObjectInstance* DroneInstance = new ObjectInstance(againDrone, GroundInstance);
	DroneInstance->SetPos(glm::vec3(0, 0, 0));
//	DroneInstance->SetRot(glm::vec3(0, 0, 0));
	DroneInstance->SetScale(glm::vec3(1, 1, 1));
	againDrone->AddInstance(DroneInstance);

	//Point Light
	//Light* newLight = new Light(nullptr,Scene);
	
	LightSys = new LightSystem( GoodGround, Scene);

	for (int i = 0; i < LIGHT_MAX - 1; i++)
	{
		//X Z : 맵 크기 내에서 랜덤하게 생성
	//	float x = (float)(dis(gen) - 0.5f) * GoodGround->GetXSize();
	//	float z = (float)(dis(gen) - 0.5f) *GoodGround->GetZSize();

		float x = -0.5 * GoodGround->GetXSize()+ 0.1* GoodGround->GetXSize()*(i % 10);
		float z = -0.25* GoodGround->GetZSize() + 0.1* GoodGround->GetZSize()*(i / 10);

	//	float x =  0.1* GoodGround->GetXSize()*(i % 10);;
	//	float z =  0.1* GoodGround->GetZSize()*(i / 10);

		//Y 최대높이 +5~8
		float y = GoodGround->GetHeightByNoise(x ,z) ;
		//float y = 10 * (float)dis(gen);

		//색 rgb 1~0.5
		float r = 1.f - 0.5f*(float)dis(gen);
		float g = 1.f - 0.5f*(float)dis(gen);
		float b = 1.f - 0.5f*(float)dis(gen);


		
		LightInstance* newLight = new LightInstance(LightSys, DroneInstance);
		newLight->SetDiff(glm::vec3(r, g, b));
	//	newLight->SetDiff(glm::vec3(1, 70, 1));
		newLight->SetPos(glm::vec3(x, y+10, z));
		float ScaleFactor = newLight->CalcLightArea();
		newLight->SetScale(glm::vec3(1, 1, 1));
		newLight->RespawnHeight = y;
		newLight->DropSpeed = 5 * (float)dis(gen);
		LightSys->AddLight(newLight);

		//Light* newLight = Scene->CreateLight(GoodGround, glm::vec4(x, y, z, 1), glm::vec3(r, g, b));
		//if (newLight != nullptr) newLight->SetRespawHeigt(y);
	}
	//델타타임을 관리하는 타이머
	TickTimer = new Timer();
	TickTimer->Init();
	pSkyBox = new SkyBox(nullptr, Scene);
	Scene->SetSkyBox(pSkyBox);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_NORMALIZE);
	//glEnable(GL_CULL_FACE);
	//MyFrameBuffers* FB = new MyFrameBuffers(4, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

//	ModelTest = new AssimpModelNode(nullptr, Scene, "./Model/Windmill", "/windmill02.obj");
	ModelTest = new AssimpObject(GoodGround, Scene, "./Model/Medievalhouse", "/med_house_final.lwo");
//	ModelTest = new AssimpModelNode(nullptr, Scene, "./Model/Monkey", "/monkey_high.obj");
//	ModelTest = new AssimpModelNode(nullptr, Scene, "./Model/Monkey", "/box.obj");
	
	//모델 트랜스폼 조절
	ModelTest->SetScale(glm::vec3(0.1, 0.1, 0.1));
	ModelTest->SetPosition(0, 0, 0);

	for (int i = 0; i < 20; i++)
	{
		float x = (float)(dis(gen) - 0.5f) * GoodGround->GetXSize();
		float z = (float)(dis(gen) - 0.5f) *GoodGround->GetZSize();
		float y = GoodGround->GetHeightByNoise(x,z);
		/*
		TransSet* WorldTransform = new TransSet();
		WorldTransform->vPos = glm::vec4(x, y, z, 1);
		WorldTransform->vRot = glm::vec3(0, 0, 0);
		WorldTransform->vScale = glm::vec3(1, 1, 1);
		*/
		ObjectInstance* NewInstance = new ObjectInstance(ModelTest , GroundInstance);
		NewInstance->SetPos(glm::vec3(x, y, z));
		//NewInstance->SetRot(glm::vec3(0, 0, 0));
		NewInstance->SetScale(glm::vec3(1, 1, 1));

		ModelTest->AddInstance(NewInstance);
	}
	
	
	SceneRenderer = new Renderer();
	
}

void display(void)
{
	// You should NOT add any more function calls in display().
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Deltatime 계산
	TickTimer->Update();
	
	//Scene Update(물리)
	Scene->Update(TickTimer->GetGameTime());
	
	//Scene Draw
	//Scene->Render();
	
	SceneRenderer->Render(Scene);

	
	
	glutSwapBuffers();
}

void Resize_Window(int w, int h)
{
	MyCamera->RatioUpdate();
	glViewport(0, 0, w, h);
}
void KeyboardInputHandler(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'P':
	case 'p':
		if (CurrentPollygonMode == GL_FILL)
			CurrentPollygonMode = GL_LINE;
		else
			CurrentPollygonMode = GL_FILL;
			
		glPolygonMode(GL_FRONT_AND_BACK, CurrentPollygonMode);
		break;
	case '1':
		MyCamera->SetMode(FREE_CAMERA);
		break;
	case '2':
		MyCamera->SetMode(ORIGIN_ROTATE_CAMERA);
		break;
	case '3':
		iMode = DRONE;
		break;
	case '4':
		iMode = DRONE_CAM;
		break;
	case '5':
		iMode = DRONE_SPOT;
		break;

	}
	if(iMode == CAM)MyCamera->KeyBoardCallBackFunc(key, x, y,TickTimer->GetGameTime());
	else if (iMode == DRONE) againDrone->DroneMoveCallBackFunc(key, TickTimer->GetGameTime());
	else if(iMode == DRONE_CAM) againDrone->CameraMoveCallBackFunc(key, TickTimer->GetGameTime());
	else if(iMode == DRONE_SPOT) againDrone->LightMoveCallBackFunc(key, TickTimer->GetGameTime());
}
void DoTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(TIME_PER_ONE_FRAME, DoTimer, 1);
}
void Mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			gui.state = STATE_ROTATING;
			gui.position_mouse = glm::ivec2(x, y);
		}
		else gui.state = STATE_NONE;
		break;
	}

}
void Motion(int x, int y)
{
	switch (gui.state)
	{
	case STATE_ROTATING:
		{	
			
			gui.offset = glm::ivec2(x, y) - gui.position_mouse;
			printf("click x:%d , y:%d\n", gui.position_mouse.x, gui.position_mouse.y);
			printf("offset x:%d , y:%d\n" , gui.offset.x , gui.offset.y);
			printf("Cur x:%d , y:%d\n", x, y);
			//위 아래인 y 는 x 축 기준회전이 필요
			MyCamera->CameraRotate(TickTimer->GetSysTime()*gui.offset.y
				, TickTimer->GetSysTime()*gui.offset.x, 0);
			gui.position_mouse = glm::ivec2(x, y);
			break;
		}
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 768);
#ifdef  __APPLE__
	glutInitDisplayMode(GLUT_RGBA | GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("Project #1 : 2009920032 Eom");
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(4, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitContextProfile(GLUT_FORWARD_COMPATIBLE);
	glutCreateWindow("Project #1 : 2009920032 Eom");
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
#endif
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(Resize_Window);
	
	glutKeyboardFunc(KeyboardInputHandler);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	glutTimerFunc(10, DoTimer ,1);
	
	glutMainLoop();
}
