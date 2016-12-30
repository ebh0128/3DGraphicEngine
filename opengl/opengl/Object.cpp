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
	//�ν��Ͻ��� ������Ʈ
	for (int i = 0; i < InstanceList.size(); i++)
	{
		glm::mat4  mTrans = glm::translate(glm::vec3(InstanceList[i]->vPos));

		//ȸ���� ��� �������� ���� ���� �߻�����
		//���߿� ���ʹϾ��̳� Local �� ���� ȸ�� �̿� ���
		glm::mat4  mRotate = glm::rotate(glm::mat4(), InstanceList[i]->vRot.x, glm::vec3(1.f, 0.f, 0.f));
		mRotate = glm::rotate(mRotate, InstanceList[i]->vRot.y, glm::vec3(0.f, 1.f, 0.f));
		mRotate = glm::rotate(mRotate, InstanceList[i]->vRot.z, glm::vec3(0.f, 0.f, 1.f));

		glm::mat4  mScale = glm::scale(InstanceList[i]->vScale);

		//S > R > T OpenGL������ �ڿ������� ����
		MatrixList[i] = mTrans * mRotate * mScale;
		MatrixList[i] = MatrixList[i];
		// �θ� * �ڽ�

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