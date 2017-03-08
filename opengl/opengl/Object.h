#pragma once


class SceneGL;
class Node;
class ObjectInstance;
//실제 사용시엔 프로토타입 패턴으로 만들어사용
class Object
{
protected:
	Node* pRoot;
	SceneGL* pScene;
	Object* mParent;

	//std::vector<TransSet*> InstanceList;
	std::vector<ObjectInstance*> InstanceList;

	//Render 할때 사용하는 인스턴스 행렬 버퍼
	std::vector<glm::mat4> MatrixList;

	std::vector<Object*> ChildList;
public:
	//루트 노드를 지정하고 생성
	Object(Node* Root,Object* Parent , SceneGL* Sce);
	void AddInstance(ObjectInstance* TsetInfo);
	virtual void Update(GLfloat dtime);
	virtual void Render();

	virtual void RenderGeoPass();
	//라이트 패스
	virtual void RenderPointLitPass() {}
	virtual void RenderDirLitPass() {}
	void AddChild(Object* pChild);
	Node* GetRoot();
	int GetInstanceNum();
	glm::mat4* GetInstanceMatrixData();
};