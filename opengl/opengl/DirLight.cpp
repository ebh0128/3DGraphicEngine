
#include "CommonHeader.h"
#include "Camera.h"
#include "IOBuffer.h"


#include "Geometry.h"
#include "Scene.h"
#include "MyFrameBuffer.h"

#include "DirLight.h"
#include "LightSystem.h"


DirLight::DirLight()
{

}
DirLight::DirLight(Object* parent, SceneGL* Scene) :Object(parent, Scene)
{
	
	//사실상 2d 이므로 2d 로사용
	Vec Vertices[] =
	{
		{ -1.00f , -1.00f , 0 },
		{ 1.00f , -1.00f , 0 },
		{ -1.00f , 1.00f , 0 },
		{ 1.00f , 1.00f , 0 },


	};
	InVec Indices[] =
	{
		{ 0 , 1 , 2 },
		{ 2 , 1 , 3 }
	};

	MeshEntry* QuadMesh = new MeshEntry((GLfloat*)Vertices, sizeof(Vertices) / sizeof(GLfloat),
		(GLuint*)Indices, sizeof(Indices) / sizeof(GLuint), nullptr);
	
	m_pModel = new Model();
	m_pModel->AddMesh(QuadMesh);
	

	//pShader = new MyShader("./Shader/Deferred_DirLight.vert", "./Shader/Deferred_DirLight.frag");
	Diffuse = glm::vec3(0.7, 0.5, 0.3);
	//Diffuse = glm::vec3(70, 50, 30);

	Ambient = Specular = glm::vec3(0.6, 0.6, 0.6);
	
	
	ForwardShaderName = m_pShaderManager->CreateShader(this, "./Shader/Deferred_DirLight.vert", "./Shader/Deferred_DirLight.frag");
	DirLightShaderName = m_pShaderManager->CreateShader(this, "./Shader/Deferred_DirLight.vert", "./Shader/Deferred_DirLight.frag");
	SSAOShaderName = m_pShaderManager->CreateShader(this, "./Shader/SSAO.vert", "./Shader/SSAO.frag");
	BlurShaderName = m_pShaderManager->CreateShader(this, "./Shader/Blur.vert", "./Shader/Blur.frag");
	HDRShaderName = m_pShaderManager->CreateShader(this, "./Shader/HDR.vert", "./Shader/HDR.frag");
	
	
	LightOrtho = glm::ortho(-350.0f, 350.0f, -350.0f, 350.0f, 0.1f, 1000.f);
		LightView = glm::lookAt(
		glm::vec3(-150, 225, 225)
		, glm::vec3(0, 0, 0)
		, glm::vec3(0, 1, 0));
	InitKernel();
	
}

void  DirLight::InitKernel()
{
	std::uniform_real_distribution<float> randomFloat(0.0, 1.0);
	std::default_random_engine fGenerator;
	for (int i = 0; i < KERNEL_SIZE*3; i+=3)
	{
		float scale = (float)i / (float)(KERNEL_SIZE);
		Kernel[i] = 2.0f * randomFloat(fGenerator)- 1.0f;
		Kernel[i + 1] = 2.0f * randomFloat(fGenerator) - 1.0f;
		Kernel[i + 2] = 2.0f * randomFloat(fGenerator) - 1.0f;
		
		// 원점에 최대한 가깝게 분포시킨것(lerp 효과)
		Kernel[i] *= (0.1f + 0.9f * scale * scale);
		Kernel[i + 1] *= (0.1f + 0.9f * scale * scale);
		Kernel[i + 2] *= (0.1f + 0.9f * scale * scale);

	}

	std::vector<glm::vec3> ssaoNoise;
	for (GLuint i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloat(fGenerator)*2.0f - 1.0f,
			randomFloat(fGenerator)*2.0f - 1.0f,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &NosieTexture);
	glBindTexture(GL_TEXTURE_2D, NosieTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
void DirLight::SetPos(glm::vec4 Lightpos)
{
	vPos = Lightpos;
}
void  DirLight::SetDiffuse(glm::vec3 dif)
{
	Diffuse = dif;
}
void  DirLight::SetSpec(glm::vec3 dif)
{
	Specular = dif;
}
void DirLight::Update(GLfloat dtime)
{
	glm::vec4 ScaleLightPos = vPos * -30.f;
	LightView = glm::lookAt(
		 glm::vec3(ScaleLightPos.x , ScaleLightPos.y , ScaleLightPos.z) 
		,glm::vec3(0,0,0)
		,glm::vec3(0,1,0));


	Object::Update(dtime);
}

// 노드 메소드 그대로 사용 >> 디버그를 위해 일단 재정의
void DirLight::Render()
{
	
	Object::Render();
}
void DirLight::ShaderParamInit(MyShader* ManagedShader)
{
	MyShader* ThisShader;
	if (ManagedShader == nullptr) return;
	else ThisShader = ManagedShader;
	ThisShader->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));

	//MVP 필요없음
	glm::mat4 VP = pScene->GetVPMatrix();
	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(TransformMat));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));


}

