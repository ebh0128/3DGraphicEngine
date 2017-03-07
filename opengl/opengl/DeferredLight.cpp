#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Object.h"
#include "DeferredLight.h"
#include "Geometry.h"
#include "Scene.h"

DeferredLight::DeferredLight(Node* Root, Object* Parent, SceneGL* Sce) : Object(Root, Parent, Sce)
{
	TransSet* WorldTransform = new TransSet();
	WorldTransform->vPos = glm::vec4(0, 50, 0, 1);
	WorldTransform->vRot = glm::vec3(0, 0, 0);
	WorldTransform->vScale = glm::vec3(10, 10, 10);
	AddInstance(WorldTransform);

}
void DeferredLight::Update(GLfloat dtime)
{
	//�� ������ �ڵ�� ���⿡
	//�ӵ�(DirectionalLight �̸� �̵��ӵ� 0)
	//float moveSpeed = vPos.w * dtime *DropSpeed;
	//if (vPos.y < 0) vPos.y = RespawnHeight;
	//vPos.y -= moveSpeed;

	
}

// �� �ȹ޴� ��ü�� �׸������̹Ƿ� ������ ������
void DeferredLight::Render()
{

}
