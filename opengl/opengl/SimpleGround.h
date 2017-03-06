#pragma once

class SimpleGround : public Node
{

public:

	SimpleGround();
	SimpleGround(Node* parent , SceneGL* Scene);
	void Update(GLfloat dtime);
	void Render();

};