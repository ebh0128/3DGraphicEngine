#pragma once
#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "Geometry.h"
#include "Scene.h"



Light::Light()
{
	
}
Light::Light(Node* parent, SceneGL* Scene) :Node(parent , Scene)
{
	Sphere* Spheremesh = new Sphere(0.5f, 8, 8);

	Mesh* mesh = new Mesh(&Spheremesh->vertices[0], Spheremesh->vertices.size()
		, &Spheremesh->indices[0], Spheremesh->indices.size(), &Spheremesh->normals[0]);

	mesh->MakeInstancingBuffer();
	AddMesh(mesh);

	delete Spheremesh;
	pShader = new MyShader("PointLight.vert", "PointLight.frag");
	//Light �н��� ���� ���̴� ����
	//pDefLitPass = new MyShader("PointLight.vert", "PointLight.frag");
	
	Diffuse = glm::vec3(1, 0, 0);
	Ambient = glm::vec3(1, 1, 1);
	Specular = glm::vec3(1, 1, 1);

	DropSpeed = 10.f;
	RespawnHeight = 20.f;
//	vScale = glm::vec3(0.1f, 0.1f, 0.1f);
	//���� ����Ʈ �߰��ϴ� �ڵ� �ֱ�(������ ����)
	

}
void Light::SetPos(glm::vec4 Lightpos)
{
	vPos = Lightpos;
}
void  Light::SetDiffuse(glm::vec3 dif)
{
	Diffuse = dif;
}
void  Light::SetSpec(glm::vec3 dif)
{
	Specular = dif;
}
void Light::Update(GLfloat dtime)
{

	Node::Update(dtime);
}
void Light::SetRespawHeigt(GLfloat Height)
{
	RespawnHeight = Height;
}

// ��� �޼ҵ� �״�� ��� >> ����׸� ���� �ϴ� ������
void Light::Render()
{
	Node::Render();
}
void Light::ShaderParamInit()
{
	pShader->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));

	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 MVP = VP*TransformMat;
	pShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	pShader->SetUniformMatrix4fv("M", glm::value_ptr(TransformMat));
	pShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));


}

void Light::RenderLitPass()
{
	// ���� ����Ʈ �н�
}
void Light::LitPassInit()
{
	
}