#pragma once
//빛ㅊ 인스턴스를 관리
class DeferredLight : public Object
{
protected:

public:
	DeferredLight(Node* Root, Object* Parent, SceneGL* Sce);


	
	void Update(GLfloat dtime);
	void Render();


};