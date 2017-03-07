#pragma once


class SceneGL;
class Node;

//실제 사용시엔 프로토타입 패턴으로 만들어사용
class Object
{
protected:
	Node* pRoot;
	SceneGL* pScene;
	Object* mParent;

	std::vector<TransSet*> InstanceList;
	std::vector<glm::mat4> MatrixList;

	std::vector<Object*> ChildList;
public:
	//루트 노드를 지정하고 생성
	Object(Node* Root,Object* Parent , SceneGL* Sce);
	void AddInstance(TransSet* TsetInfo);
	virtual void Update(GLfloat dtime);
	virtual void Render();

	virtual void RenderGeoPass();
	virtual void RenderLitPass();

	void AddChild(Object* pChild);
	Node* GetRoot();
	int GetInstanceNum();
	glm::mat4* GetInstanceMatrixData();
};