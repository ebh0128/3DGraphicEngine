#pragma once

class Object;

//실제 월드에서 움직이고 그려지는 객체의 인스턴스
class ObjectInstance
{
protected:
	Object* Original;
	//인스턴스 끼리의 부모관계 (행렬에 주로 영향)
	ObjectInstance* Parents;

	glm::vec3 vPos;
	glm::vec3 vRot;
	glm::vec3 vScale;

	glm::mat4 mWorld;

	//부모와 자신의 곱(드로우시 이걸 사용)
	glm::mat4 mParXWorld;

	//애니메이션 관련 상수들 추가

public:
	ObjectInstance(Object* Ori , ObjectInstance* _Parents = nullptr);

	//오브젝트에서 호출(별다른 개별 로직 없으면 그대로 사용)
	//개별 동작을 행렬에 적용하는 로직
	//상속해서 재정의 하더라도 반드시 호출
	virtual void Update(float dtime);
	//virtual void Render();

	glm::mat4 GetMat() { return mParXWorld; }
	glm::mat4 GetWorld() { return mWorld; }

	void SetPos(glm::vec3 pos) { vPos = pos; }
	void SetRot(glm::vec3 rot) { vRot = rot; }
	void SetScale(glm::vec3 scale) { vScale = scale; }

	glm::vec3 GetPos() { return vPos; }
	glm::vec3 GetRot() { return vRot; }
	glm::vec3 GetScale() { return vScale; }
};