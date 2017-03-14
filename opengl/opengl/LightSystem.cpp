#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"


#include "LightSystem.h"
#include "Geometry.h"
#include "Scene.h"
 
#include "MyFrameBuffer.h"

LightInstance::LightInstance(Object* Ori, ObjectInstance* _Parents) :ObjectInstance(Ori,_Parents)
{
	Ambient = Specular = Diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
	//Ambient = Specular = Diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
	
	DiffuseFactor = 1;
	AmbientFactor = 1;
	SpecularFactor = 1;

	Attnuation.Constant = 0.3f;
	Attnuation.exp = 0.02f;
	Attnuation.Linear = 0.05f;


}
void LightInstance::Update(float dtime)
{
	//포인트만 해당
	/*float moveSpeed =  dtime *DropSpeed;
	if (vPos.y < RespawnHeight - 30)
	{
		vPos.y = RespawnHeight;
		DropSpeed = 0;
	}
	vPos.y -= moveSpeed;

	*/

	ObjectInstance::Update(dtime);
}

float LightInstance::CalcLightArea()
{
	float MaxRGB = fmax(fmax(Diffuse.r, Diffuse.g), Diffuse.b);
	float DiffuseIntense = 1.0f;
	float ret = (-Attnuation.Linear
		+ sqrtf(Attnuation.Linear *Attnuation.Linear - 4 * Attnuation.exp*(Attnuation.Constant - 256 * MaxRGB*DiffuseIntense)))
		/ (2 * Attnuation.exp);
	return ret;
}





////////////////////////////////////////////////////////////////////////////////////



LightSystem::LightSystem( Object* Parent, SceneGL* Sce) : Object( Parent, Sce)
{
	Sce->SetLightSystem(this);

	Sphere* Spheremesh = new Sphere(0.5f, 16, 16);

	MeshEntry* mesh = new MeshEntry(&Spheremesh->vertices[0], Spheremesh->vertices.size()
		, &Spheremesh->indices[0], Spheremesh->indices.size(), &Spheremesh->normals[0]);

	//mesh->MakeInstancingBuffer();
	
	m_pModel = new Model();
	m_pModel->AddMesh(mesh);
	m_pModel->MakeInstancingBuffer();
	m_pModel->MakeInstancingBuffer();

	delete Spheremesh;

	pShader = new MyShader("PointLight.vert", "PointLight.frag");
	//Light 패스를 위한 쉐이더 정의
	pDefPtLitPass = new MyShader("./Shader/Deferred_PointLight.vert", "./Shader/Deferred_PointLight.frag");
	nullShader = new MyShader("./Shader/Deferred_PointLight.vert", "./Shader/Deferred_PointLight.frag");


	//	vScale = glm::vec3(0.1f, 0.1f, 0.1f);
	//씬에 라이트 추가하는 코드 넣기(씬에서 관리)
	int strSize = sizeof(PaddingLight);
	AddUBO(nullptr, strSize*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0, pDefPtLitPass);

}
void LightSystem::AddInstanceCallBack()
{
	LightAttributeList.push_back(glm::mat4());
}

void LightSystem::InstanceDataSetting()
{
	int InstanceCount = GetInstanceNum();

	//빛 정보 넘겨주기
	//행렬로
	for (int i = 0; i<LightAttributeList.size(); i++)
	{
		LightInstance* LightIns = (LightInstance*)InstanceList[i];

		glm::vec4 LightPos = glm::vec4(LightIns->GetPos() , 1);
		glm::vec4 LightDif = glm::vec4(LightIns->GetDif(), 1);
		glm::vec4 LightAmb = glm::vec4(LightIns->GetAmbi(), 1);
		LightAttnuation LitAtn = LightIns->GetAttnuation();
		glm::vec4 vLightatn = glm::vec4(LitAtn.Constant, LitAtn.Linear, LitAtn.exp, 1);

		LightAttributeList[i][0] = LightPos;
		LightAttributeList[i][1] = LightDif;
		LightAttributeList[i][2] = LightAmb;
		LightAttributeList[i][3] = vLightatn;
	}

	m_pModel->SetInstanceBufferData(LightAttributeList.data(), 1);
	Object::InstanceDataSetting();
}
void LightSystem::Update(GLfloat dtime)
{
	Object::Update(dtime);
	
}

// 빛 안받는 객체로 그릴예정이므로 완전히 재정의
void LightSystem::Render()
{
	Object::Render();
}
void  LightSystem::AddLight(LightInstance* pnew)
{
	//씬에다 등록
	pScene->AddLight(pnew);
	AddInstance(pnew);
}

void LightSystem::ShaderParamInit()
{
	glm::vec4 TempDiff = glm::vec4(1, 1, 1, 1);
	pShader->SetUniform4fv("DiffuseCol", glm::value_ptr(TempDiff));

	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 MVP = VP*TransformMat;
	pShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));

	//광원 그리기용 스케일
	glm::vec3 PtScale = glm::vec3(0.05f, 0.05f, 0.05f );
	glm::mat4 NewTrans = TransformMat*glm::scale(PtScale);
	pShader->SetUniformMatrix4fv("M", glm::value_ptr(NewTrans));
	pShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));



}

void LightSystem::RenderPointLitPass()
{
	if (!pDefPtLitPass)  return;
	pDefPtLitPass->ApplyShader();

	
		// 변환 행렬 쉐이더 전송
	PointLitPassInit();
	
	if (GetInstanceNum() == 0) m_pModel->Render();
	else 
	{
		InstanceDataSetting();
		m_pModel->RenderInstance(GetInstanceNum());
		
	}
	
	
	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->RenderPointLitPass();
	}
}

void LightSystem::PointLitPassInit()
{

//	pDefPtLitPass->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));

	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 MVP = VP*TransformMat;

	pDefPtLitPass->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	pDefPtLitPass->SetUniformMatrix4fv("M", glm::value_ptr(TransformMat));
	pDefPtLitPass->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	pDefPtLitPass->SetUniformMatrix4fv("V", glm::value_ptr(V));

	glm::mat4 ParentsTransform = InstanceList[0]->GetParentsMat();;
	pDefPtLitPass->SetUniformMatrix4fv("LightTransMat", glm::value_ptr(ParentsTransform));


	glm::vec2 ScreenSize = glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	pDefPtLitPass->SetUniform2fv("gScreenSize", glm::value_ptr(ScreenSize));

	glm::vec4 CameraPos = pScene->GetCurrentCamPos();
	pDefPtLitPass->SetUniform4fv("gEyeWorldPos", glm::value_ptr(CameraPos));

	//gBuffer 텍스쳐 보내기
	pDefPtLitPass->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	pDefPtLitPass->SetUniform1i("gColorMap", DeferredRenderBuffers::TEXTURE_TYPE_DIFFUSE);
	pDefPtLitPass->SetUniform1i("gNormalMap", DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);

	LightList* DataforShader = pScene->GetLightSrouceArray();
	GLuint Size = DataforShader->Count * sizeof(PaddingLight);
	//meshes[i]->UpdateUBO(DataforShader, Size+ sizeof(GLuint), 0);
	UpdateUBO(DataforShader, sizeof(GLuint), 0);

	// std140 stride 16
	UpdateUBO(DataforShader, Size, 12);
	//	UpdateUBO(DataforShader, Size, 16);

	for (int i = 0; i < InstanceList.size(); i++)
	{
		LightInstance* LightIns = ((LightInstance*)InstanceList[i]);
		LightIns->SetScale(glm::vec3(LightIns->CalcLightArea()));
	}
}

void LightSystem::RenderStencilPass()
{
	if (!nullShader)  return;
	nullShader->ApplyShader();

	
		// 변환 행렬 쉐이더 전송
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 MVP = VP*TransformMat;
	pDefPtLitPass->SetUniformMatrix4fv("M", glm::value_ptr(TransformMat));
	pDefPtLitPass->SetUniformMatrix4fv("VP", glm::value_ptr(VP));

	if (GetInstanceNum() == 0) m_pModel->Render();
	else
	{
		InstanceDataSetting();
		m_pModel->RenderInstance(GetInstanceNum());

	}
	
	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->RenderPointLitPass();
	}
}