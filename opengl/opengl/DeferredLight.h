#pragma once
//���� �ν��Ͻ��� ����
class DeferredLight : public Object
{
protected:

public:
	DeferredLight(Node* Root, Object* Parent, SceneGL* Sce);


	
	void Update(GLfloat dtime);
	void Render();


};