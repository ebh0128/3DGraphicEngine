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

	AddMesh(mesh);

	delete Spheremesh;
	pShader = new MyShader("PointLight.vert", "PointLight.frag");
	//Light 패스를 위한 쉐이더 정의
	//pDefLitPass = new MyShader("PointLight.vert", "PointLight.frag");
	
	Diffuse = glm::vec3(1, 0, 0);
	Ambient = glm::vec3(1, 1, 1);
	Specular = glm::vec3(1, 1, 1);

	DropSpeed = 10.f;
	RespawnHeight = 20.f;
//	vScale = glm::vec3(0.1f, 0.1f, 0.1f);
	//씬에 라이트 추가하는 코드 넣기(씬에서 관리)
	

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
	//빛 움직임 코드는 여기에
	//속도(DirectionalLight 이면 이동속도 0)
	float moveSpeed = vPos.w * dtime *DropSpeed;
	if (vPos.y < 0) vPos.y = RespawnHeight;
	vPos.y -= moveSpeed;

	Node::Update(dtime);
}
void Light::SetRespawHeigt(GLfloat Height)
{
	RespawnHeight = Height;
}

// 노드 메소드 그대로 사용 >> 디버그를 위해 일단 재정의
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

}