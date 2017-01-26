
///////////////////////////////////////////////////////////////////////
#include "CommonHeader.h"

#include "Camera.h"
#include "myShader.h"
#include "Timer.h"
#include "Mesh.h"
#include "SimpleGround.h"
#include "PatchedGround.h"
#include "Light.h"
#include "Scene.h"
#include "SkyBox.h"
#include "MyFrameBuffer.h"
#include "AssimpModelNode.h"
#include "Object.h"
#include "Drone.h"
//60������ ������ 1�����Ӵ� �и��� 16
#define TIME_PER_ONE_FRAME 10


ShaderManager* ShaderManager::pInstance = nullptr;

GLuint	vao;
GLuint	vbo[2], ebo;
GLuint	h_prog;
static GLuint	attrib_position = 0;
static GLuint	attrib_color = 7;
enum VBO_Bufs { VBO_POSITION = 0,  NUM_VBOS = 2 };

// ���콺 ������ ����
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
AssimpModelNode* ModelTest;
Object* GroundObject;
Object* ModelObject;
Drone* againDrone;

void init(void)
{
	iMode = CAM;
	//DevIL �ʱ�ȭ
	ilInit();
	ilutRenderer(ILUT_OPENGL);
	//�� �����
	Scene = new SceneGL();
	//ī�޶� ����
	MyCamera = new Camera(glm::vec3(0, 128, 128), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	//�� ����
	
	GoodGround = new PatchedGround(nullptr, Scene, 102, 64.f);
	//GoodGround->Create(256, 256, 0.5f, 10, 10);
	GoodGround->Create(512, 512, 0.5f, 10, 10);

	GroundObject = new Object(GoodGround,nullptr,Scene);

	Scene->AddCam(MyCamera);
	Scene->SetRoot(GroundObject);
	
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

	//�¾籤
	Light* pDirLight = Scene->CreateLight(GoodGround, glm::vec4(10.f, 30.0f, 20.f, 0), glm::vec3(0.3f, 0.3f, 0.7f));
	Scene->SetDirectionalLight(pDirLight);
	pDirLight->SetSpec(glm::vec3(0, 0, 0));
	//�� �� ��!(�����ϰ�)
	for (int i = 0; i < LIGHT_MAX - 1; i++)
	{
		//X Z : �� ũ�� ������ �����ϰ� ����
		float x = (float)(dis(gen)-0.5f) * GoodGround->GetXSize();
		float z = (float)(dis(gen)-0.5f) *GoodGround->GetZSize() ;
		
		//Y �ִ���� +5~8
		float y = GoodGround->GetMaxHeight() +5.0f + (float)(3 * dis(gen));
		//float y = 10 * (float)dis(gen);

		//�� rgb 1~0.5
		float r = 1.f - 0.5f*(float)dis(gen);
		float g = 1.f - 0.5f*(float)dis(gen);
		float b = 1.f - 0.5f*(float)dis(gen);
		
	
		Light* newLight =Scene->CreateLight(GoodGround, glm::vec4(x, y, z, 1), glm::vec3(r, g, b));
		if (newLight != nullptr) newLight->SetRespawHeigt(y);
	}
	//��ŸŸ���� �����ϴ� Ÿ�̸�
	TickTimer = new Timer();
	TickTimer->Init();
	pSkyBox = new SkyBox(nullptr, Scene);
	Scene->SetSkyBox(pSkyBox);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_NORMALIZE);
	//glEnable(GL_CULL_FACE);
	DeferredRenderBuffers* FB = new DeferredRenderBuffers(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

//	ModelTest = new AssimpModelNode(GoodGround, Scene, "./Model/Windmill", "/windmill02.obj");
	ModelTest = new AssimpModelNode(GoodGround, Scene, "./Model/Medievalhouse", "/med_house_final.lwo");
//	ModelTest = new AssimpModelNode(GoodGround, Scene, "./Model/Monkey", "/monkey_high.obj");
//	ModelTest = new AssimpModelNode(GoodGround, Scene, "./Model/Monkey", "/box.obj");
	
	//�� Ʈ������ ����
	ModelTest->SetScale(glm::vec3(0.1, 0.1, 0.1));
	ModelTest->SetPosition(0, 2, 0);
	ModelObject = new Object(ModelTest, GroundObject, Scene);
	for (int i = 0; i < 20; i++)
	{
		float x = (float)(dis(gen) - 0.5f) * GoodGround->GetXSize();
		float z = (float)(dis(gen) - 0.5f) *GoodGround->GetZSize();
		float y = GoodGround->GetHeightByNoise(x,z);

		TransSet* WorldTransform = new TransSet();
		WorldTransform->vPos = glm::vec4(x, y, z, 1);
		WorldTransform->vRot = glm::vec3(0, 0, 0);
		WorldTransform->vScale = glm::vec3(1, 1, 1);
		ModelObject->AddInstance(WorldTransform);
	}
	AssimpModelNode* DroneRoot = new AssimpModelNode(NULL, Scene, "./Model/Monkey", "/monkey_high.obj");
	DroneRoot->SetNoTexture();
	againDrone = new Drone(DroneRoot, GroundObject, Scene);
	
}

void display(void)
{
	// You should NOT add any more function calls in display().
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Deltatime ���
	TickTimer->Update();
	
	//Scene Update(����)
	Scene->Update(TickTimer->GetGameTime());
	
	//Scene Draw
	Scene->Render();
	
	Scene->ChangeCamera(1);
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	GLint w = m_viewport[2], h = m_viewport[3];
	glViewport(w*3/4, 0, w, h *1/4);


	Scene->Render();
	glViewport(m_viewport[0], m_viewport[1], w, h );

	Scene->ChangeCamera(0);
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
			//�� �Ʒ��� y �� x �� ����ȸ���� �ʿ�
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
	glutInitWindowSize(512, 512);
#ifdef  __APPLE__
	glutInitDisplayMode(GLUT_RGBA | GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("Project #1 : 2009920032 Eom");
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 3);
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
