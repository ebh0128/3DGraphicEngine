#include "CommonHeader.h"
#include "ObjectInstance.h"


ObjectInstance::ObjectInstance(Object* Ori, ObjectInstance* _Parents)
{
	Original = Ori;
	if (_Parents != nullptr)
	{
		SetParents(_Parents);
		_Parents->AddChild(this);
	}
	mRot = glm::mat4();
}


void ObjectInstance::Update(float dtime)
{
	Original->Update(dtime);

	glm::mat4  mTrans = glm::translate(vPos);

	/*
	glm::mat4  mRotate = glm::rotate(glm::mat4(), vRot.x, glm::vec3(1.f, 0.f, 0.f));
	mRotate = glm::rotate(mRotate, vRot.y, glm::vec3(0.f, 1.f, 0.f));
	mRotate = glm::rotate(mRotate, vRot.z, glm::vec3(0.f, 0.f, 1.f));
	*/
	glm::mat4  mScale = glm::scale(vScale);


	//S > R > T OpenGL에서는 뒤에서부터 읽음
	mWorld = mTrans * mRot * mScale;
	
	if (Parents != nullptr) mParXWorld = GetMat();
	else mParXWorld = mWorld;

	//mParXWorld = mWorld;
	std::list<ObjectInstance*>::iterator itor;
	
	for (itor = pChildrenList.begin(); itor != pChildrenList.end(); itor++)
	{
		(*itor)->Update(dtime);
	}
}
void ObjectInstance::Rotation(glm::vec3 DeltaRot)
{
	glm::mat4  mDeletaRot = glm::rotate(glm::mat4(), DeltaRot.x, glm::vec3(1.f, 0.f, 0.f));
	mDeletaRot = glm::rotate(mDeletaRot, DeltaRot.y, glm::vec3(0.f, 1.f, 0.f));
	mDeletaRot = glm::rotate(mDeletaRot, DeltaRot.z, glm::vec3(0.f, 0.f, 1.f));

	mRot = mDeletaRot*mRot;


}


void ObjectInstance::SetParents(ObjectInstance* parent)
{
	if (Parents != nullptr)
	{
		Parents->RemovChild(this);
	}
	Parents = parent;

}
void ObjectInstance::AddChild(ObjectInstance* Child)
{
	pChildrenList.push_back(Child);
}
void ObjectInstance::RemovChild(ObjectInstance* target)
{
	//찾아서 삭제
	pChildrenList.remove(target);
}

glm::mat4 ObjectInstance::GetMat()
{ 
	return (Parents == nullptr) ? mWorld : Parents->GetMat()*mWorld;
}
glm::mat4 ObjectInstance::GetParentsMat()
{
	return Parents->GetMat();
}

glm::mat4  ObjectInstance::GetMatWithoutScale()
{
	glm::mat4  mTrans = glm::translate(vPos);

	
	glm::mat4 LightTransform = mTrans*mRot;

//	return (Parents == nullptr) ? LightTransform : Parents->GetMatWithoutScale()*LightTransform;
	return (Parents == nullptr) ? glm::mat4() : Parents->mWorld;
}
