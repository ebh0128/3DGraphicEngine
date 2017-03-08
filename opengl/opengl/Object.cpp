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
	//행렬 버퍼 크기 증가
	MatrixList.push_back(glm::mat4());
}
void Object::Update(GLfloat dtime)
{
	//인스턴스들 업데이트
	for (int i = 0; i < InstanceList.size(); i++)
	{
		InstanceList[i]->Update(dtime);
	}

	//리소스 (모델공간) 업데이트
	pRoot->Update(dtime);

	//자식 오브젝트 업데이트
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
	if (MatrixList.size() != InstanceList.size()) printf("오류 :인스턴스와 행렬버퍼의 크기가 다름\n");
	//값만 복사해놓고 넘겨줌
	for (int i = 0; i < MatrixList.size(); i++)
	{
		MatrixList[i] = InstanceList[i]->GetMat();
	}
	return MatrixList.data();
}