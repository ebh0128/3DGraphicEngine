#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Scene.h"
#include "Object.h"



Object::Object(Node* Root, Object* Parent, SceneGL* Sce)
{
	pRoot = Root;
	pRoot->SetObject(this);
	mParent = Parent;
	if(Parent != nullptr) Parent->AddChild(this);
	pScene = Sce;
}
void Object::AddInstance(TransSet* TsetInfo)
{
	if (TsetInfo == nullptr) return;

	InstanceList.push_back(TsetInfo);
	MatrixList.push_back(glm::mat4());
}
void Object::Update(GLfloat dtime)
{
	//인스턴스들 업데이트
	for (int i = 0; i < InstanceList.size(); i++)
	{
		glm::mat4  mTrans = glm::translate(glm::vec3(InstanceList[i]->vPos));

		//회전의 경우 짐벌락등 여러 문제 발생가능
		//나중에 쿼터니언이나 Local 축 기준 회전 이용 고려
		glm::mat4  mRotate = glm::rotate(glm::mat4(), InstanceList[i]->vRot.x, glm::vec3(1.f, 0.f, 0.f));
		mRotate = glm::rotate(mRotate, InstanceList[i]->vRot.y, glm::vec3(0.f, 1.f, 0.f));
		mRotate = glm::rotate(mRotate, InstanceList[i]->vRot.z, glm::vec3(0.f, 0.f, 1.f));

		glm::mat4  mScale = glm::scale(InstanceList[i]->vScale);

		//S > R > T OpenGL에서는 뒤에서부터 읽음
		MatrixList[i] = mTrans * mRotate * mScale;
		MatrixList[i] = MatrixList[i];
		// 부모 * 자식

	}
	pRoot->Update(dtime);

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
	return MatrixList.data();
}