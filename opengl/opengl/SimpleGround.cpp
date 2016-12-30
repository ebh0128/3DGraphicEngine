#pragma once
#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Scene.h"
#include "SimpleGround.h"



SimpleGround::SimpleGround()
{
}
SimpleGround::SimpleGround(Node* parent, SceneGL* Scene) : Node(parent, Scene)
{
	GLfloat vertices[] = {
		-10, 0, -10 ,
		10, 0, -10 ,
		-10,  0, 10 ,
		10,  0, 10 ,

	};

	int VertCont = sizeof(vertices) / sizeof(GLfloat);

	// ����(���Ž� �ڵ�)
	GLfloat* pVertices = new GLfloat[VertCont];
	memcpy(pVertices, vertices, sizeof(vertices));


	GLuint	indices[] = {
		0 ,1 ,2 ,
		2 ,1 ,3 ,
	};
	int indCont = sizeof(indices) / sizeof(GLuint);
	GLuint* pindices = new GLuint[indCont];
	memcpy(pindices, indices, sizeof(indices));

	////
	//Normal ����(���Ž� �ڵ�)
	GLfloat *pNormal = new GLfloat[VertCont];
	for (int i = 0; i < VertCont; i += 3)
	{
		pNormal[i] = 0.f; pNormal[i + 1] = 1.f; pNormal[i + 2] = 0.f;
	}
	Mesh* GroundMesh = new Mesh(pVertices, VertCont, pindices, indCont, pNormal);

	AddMesh(GroundMesh);
	//���̴� �ʱ�ȭ
	pShader = new MyShader();
	pShader->build_program_from_files("BasicShader.vert", "BasicShader.frag");

}
void SimpleGround::Update(GLfloat dtime)
{
	//���⿡ ������Ʈ

	//
	Node::Update(dtime);
}
void SimpleGround::Render()
{
	if (pShader) pShader->ApplyShader();
	
	//���� ���� �����̱⿡ World Ʈ������ ����

	Node::Render();
}