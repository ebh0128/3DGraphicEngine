#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Scene.h"
#include "ObjectInstance.h"
#include "Object.h"



Object::Object(Node* Root, Object* Parent, SceneGL* Sce)
{
	pRoot = Root;
	pRoot->SetObject(this);
	mParent = Parent;
	if(Parent != nullptr) Parent->AddChild(this);
	pScene = Sce;
}
void Object::AddInstance(ObjectInstance* TsetInfo)
{
	if (TsetInfo == nullptr) return;

	InstanceList.push_back(TsetInfo);
	//��� ���� ũ�� ����
	MatrixList.push_back(glm::mat4());
}
void Object::Update(GLfloat dtime)
{
	//�ν��Ͻ��� ������Ʈ
	for (int i = 0; i < InstanceList.size(); i++)
	{
		InstanceList[i]->Update(dtime);
	}

	//���ҽ� (�𵨰���) ������Ʈ
	pRoot->Update(dtime);

	//�ڽ� ������Ʈ ������Ʈ
	for (int i = 0; i < ChildList.size(); i++)
	{
		ChildList[i]->Update(dtime);
	}
}
void Object::Render()
{
	pRoot->Render();
	for (int i = 0; i < ChildList.size(); i++)
	{
		ChildList[i]->Render();
	}
}

void Object::RenderGeoPass()
{
	pRoot->RenderGeoPass();
	for (int i = 0; i < ChildList.size(); i++)
	{
		ChildList[i]->RenderGeoPass();
	}
}


void Object::AddChild(Object* pChild)
{
	ChildList.push_back(pChild);
}
Node* Object::GetRoot()
{
	return pRoot;
}
int Object::GetInstanceNum()
{
	return InstanceList.size();
}
glm::mat4* Object::GetInstanceMatrixData()
{
	if (MatrixList.size() != InstanceList.size()) printf("���� :�ν��Ͻ��� ��Ĺ����� ũ�Ⱑ �ٸ�\n");
	//���� �����س��� �Ѱ���
	for (int i = 0; i < MatrixList.size(); i++)
	{
		MatrixList[i] = InstanceList[i]->GetMat();
	}
	return MatrixList.data();
}