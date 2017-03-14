#pragma once

class Object;

//���� ���忡�� �����̰� �׷����� ��ü�� �ν��Ͻ�
class ObjectInstance
{
protected:
	Object* Original;
	//�ν��Ͻ� ������ �θ���� (��Ŀ� �ַ� ����)
	ObjectInstance* Parents;
	//�ڽ��� ����� �ٲ������
	std::list<ObjectInstance*> pChildrenList;

	glm::vec3 vPos;
	//glm::vec3 vRot;
	//glm::vec3 vDeltaRot;
	glm::mat4 mRot;


	glm::vec3 vScale;

	glm::mat4 mWorld;

	//�θ�� �ڽ��� ��(��ο�� �̰� ���)
	glm::mat4 mParXWorld;

	//�ִϸ��̼� ���� ����� �߰�

public:
	ObjectInstance(Object* Ori , ObjectInstance* _Parents = nullptr);

	//������Ʈ���� ȣ��(���ٸ� ���� ���� ������ �״�� ���)
	//���� ������ ��Ŀ� �����ϴ� ����
	//����ؼ� ������ �ϴ��� �ݵ�� ȣ��

	virtual void Update(float dtime);
	//virtual void Render();

	void SetParents(ObjectInstance* parent);
	ObjectInstance* GetParents() { return Parents; }
	void AddChild(ObjectInstance* Child);
	void RemovChild(ObjectInstance* target);

	glm::mat4 GetMat();
	glm::mat4 GetMatWithoutScale();
	glm::mat4 GetWorld() { return mWorld; }
	glm::mat4 GetParentsMat();

	void SetPos(glm::vec3 pos) { vPos = pos; }
	//void SetRot(glm::vec3 rot) { vRot = rot; }
	void Rotation(glm::vec3 DeltaRot);
	void SetScale(glm::vec3 scale) { vScale = scale; }

	glm::vec3 GetPos() { return vPos; }
	//glm::vec3 GetRot() { return vRot; }
	glm::vec3 GetScale() { return vScale; }
};