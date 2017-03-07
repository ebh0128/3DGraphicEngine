#include "CommonHeader.h"
#include "Object.h"
#include "ObjectInstance.h"


void ObjectInstance::Update(float dtime)
{
	glm::mat4  mTrans = glm::translate(vPos);

	glm::mat4  mRotate = glm::rotate(glm::mat4(), vRot.x, glm::vec3(1.f, 0.f, 0.f));
	mRotate = glm::rotate(mRotate, vRot.y, glm::vec3(0.f, 1.f, 0.f));
	mRotate = glm::rotate(mRotate, vRot.z, glm::vec3(0.f, 0.f, 1.f));

	glm::mat4  mScale = glm::scale(vScale);

	//S > R > T OpenGL에서는 뒤에서부터 읽음
	mWorld = mTrans * mRotate * mScale;

	if (Parents != nullptr) mParXWorld = mWorld * Parents->GetMat();
	else mParXWorld = mWorld;
}

ObjectInstance::ObjectInstance(Object* Ori, ObjectInstance* _Parents)
{
	Original = Ori;
	Parents = _Parents;
}