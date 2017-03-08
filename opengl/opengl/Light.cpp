#pragma once
#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "Geometry.h"
#include "Scene.h"
#include "Object.h"
#include "MyFrameBuffer.h"




Light::Light()
{
	
}
Light::Light(Node* parent, SceneGL* Scene) :Node(parent , Scene)
{
	Sphere* Spheremesh = new Sphere(0.5f, 16, 16);

	Mesh* mesh = new Mesh(&Spheremesh->vertices[0], Spheremesh->vertices.size()
		, &Spheremesh->indices[0], Spheremesh->indices.size(), &Spheremesh->normals[0]);

	mesh->MakeInstancingBuffer();
	AddMesh(mesh);

	delete Spheremesh;
	pShader = new MyShader("PointLight.vert", "PointLight.frag");
	//Light �н��� ���� ���̴� ����
	pDefPtLitPass = new MyShader("./Shader/Deferred_PointLight.vert", "./Shader/Deferred_PointLight.frag");
	
	Diffuse = glm::vec3(1, 1, 1);
	Ambient = glm::vec3(1, 1, 1);
	Specular = glm::vec3(1, 1, 1);

	DropSpeed = 10.f;
	RespawnHeight = 20.f;
//	vScale = glm::vec3(0.1f, 0.1f, 0.1f);
	//���� ����Ʈ �߰��ϴ� �ڵ� �ֱ�(������ ����)
	int strSize = sizeof(PaddingLight);
	AddUBO(nullptr, strSize*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0 , pDefPtLitPass);

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

void Light::RenderPointLitPass()
{
	if (!pDefPtLitPass)  return;
	pDefPtLitPass->ApplyShader();

	for (GLuint i = 0; i<meshes.size(); i++)
	{
		// ��ȯ ��� ���̴� ����
		PointLitPassInit();
		if (pObj->GetInstanceNum() == 0) meshes[i]->Render(1);
		else meshes[i]->Render(pObj->GetInstanceMatrixData(), pObj->GetInstanceNum(), 1);

	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->RenderPointLitPass();
	}
}
void Light::PointLitPassInit()
{
	pDefPtLitPass->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));

	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 MVP = VP*TransformMat;
	pDefPtLitPass->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	pDefPtLitPass->SetUniformMatrix4fv("M", glm::value_ptr(TransformMat));
	pDefPtLitPass->SetUniformMatrix4fv("VP", glm::value_ptr(VP));

	glm::vec2 ScreenSize = glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	pDefPtLitPass->SetUniform2fv("gScreenSize", glm::value_ptr(ScreenSize));
	
	glm::vec4 CameraPos = pScene->GetCurrentCamPos();
	pDefPtLitPass->SetUniform4fv("gEyeWorldPos", glm::value_ptr(CameraPos));
	
	//gBuffer �ؽ��� ������
	pDefPtLitPass->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	pDefPtLitPass->SetUniform1i("gColorMap", DeferredRenderBuffers::TEXTURE_TYPE_DIFFUSE);
	pDefPtLitPass->SetUniform1i("gNormalMap", DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);

	LightList* DataforShader = pScene->GetLightSrouceArray();
	GLuint Size = DataforShader->Count * sizeof(PaddingLight);
	//meshes[i]->UpdateUBO(DataforShader, Size+ sizeof(GLuint), 0);
	UpdateUBO(DataforShader, sizeof(GLuint), 0);

	// std140 stride 16
	UpdateUBO(DataforShader, Size, 12);
//	UpdateUBO(DataforShader, Size, 16);

}
void Light::RenderDirLitPass()
{
	
}

void Light::DirLitPassInit()
{

}
