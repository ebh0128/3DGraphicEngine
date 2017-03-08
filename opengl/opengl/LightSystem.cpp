#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Object.h"
#include "ObjectInstance.h"
#include "LightSystem.h"
#include "Geometry.h"
#include "Scene.h"

LightInstance::LightInstance(Object* Ori, ObjectInstance* _Parents) :ObjectInstance(Ori,_Parents)
{
	Ambient = Specular = Diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
	//Ambient = Specular = Diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
	
	DiffuseFactor = 1;
	AmbientFactor = 1;
	SpecularFactor = 1;

	Attnuation.Constant = 0.3f;
	Attnuation.exp = 0.05f;
	Attnuation.Linear = 0.05f;
}
void LightInstance::Update(float dtime)
{
	//포인트만 해당
	float moveSpeed =  dtime *DropSpeed;
	if (vPos.y < 0) vPos.y = RespawnHeight;
	vPos.y -= moveSpeed;


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

LightSystem::LightSystem(Node* Root, Object* Parent, SceneGL* Sce) : Object(Root, Parent, Sce)
{
	Sce->SetLightSystem(this);
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

void LightSystem::RenderPointLitPass()
{
	pRoot->RenderPointLitPass();
}
void LightSystem::RenderDirLitPass()
{
	pRoot->RenderDirLitPass();
}
