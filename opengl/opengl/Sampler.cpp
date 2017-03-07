#pragma once
#include "CommonHeader.h"
#include "MyShader.h"
#include "Texture.h"
#include "Sampler.h"


Sampler::Sampler()
{
	glGenSamplers(1, &SamplerID);

	m_iWrapS = GL_REPEAT;
	m_iWrapT = GL_REPEAT;
	m_iMag = GL_LINEAR;
	m_iMin = GL_LINEAR;
}
Sampler::Sampler(const char* ImagePath, GLuint Channel, GLint ShaderLoc, GLint DefShaderLoc) :Sampler()
{

	CreateTexture(ImagePath, Channel, ShaderLoc, DefShaderLoc);
}

void Sampler::CreateTexture(const char* ImagePath, GLuint Channel , GLint ShaderLoc, GLint DefShaderLoc)
{
	
	Texture* pNew = new Texture(ImagePath, Channel);
	pNew->SetShaderValue(ShaderLoc);
	if (DefShaderLoc >= 0) pNew->SetShaderValue(DefShaderLoc, 1);
	AddTexture(pNew);
	glBindSampler(Channel, SamplerID);
}

void Sampler::CreateTextureByData(GLfloat* Data, int Width, int Height, GLuint Channel, GLint ShaderLoc, GLint DefShaderLoc)
{

	m_iWrapS = GL_CLAMP_TO_BORDER;
	m_iWrapT = GL_CLAMP_TO_BORDER;
	m_iMag = GL_NEAREST;
	m_iMin = GL_NEAREST;

	Texture* pNew = new Texture();
	pNew->CreateTextureByData(Data, Width, Height, Channel);
	pNew->SetShaderValue(ShaderLoc);
	if (DefShaderLoc >= 0) pNew->SetShaderValue(DefShaderLoc, 1);
	AddTexture(pNew);
	glBindSampler(Channel, SamplerID);
}


void Sampler::CreateCubeMapTexture(CubeMapTexturePathInfo* ImagePath, GLuint Channel, GLint ShaderLoc)
{
	//glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	//m_iWrapS = GL_CLAMP_TO_EDGE;
	//m_iWrapT = GL_CLAMP_TO_EDGE;

	Texture* pNew = new Texture();
	pNew->CreateCubeMapTexture(ImagePath, Channel);
	pNew->SetShaderValue(ShaderLoc);
	AddTexture(pNew);
	glBindSampler(Channel, SamplerID);

}

void Sampler::AddTexture(Texture* pNewtex)
{
	TextureList.push_back(pNewtex);
}

void Sampler::ApplySampler(int isDeferred)
{
	glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_S, m_iWrapS);
	glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_T, m_iWrapT);
	glSamplerParameteri(SamplerID, GL_TEXTURE_MAG_FILTER, m_iMag);
	glSamplerParameteri(SamplerID, GL_TEXTURE_MIN_FILTER, m_iMin);

	for (int i = 0; i < TextureList.size(); i++)
	{
		TextureList[i]->ApplyTexture(isDeferred);


	}
}
void Sampler::UpdateTexture(int index, GLuint Width, GLuint Height, GLuint Format, GLuint DataType, GLfloat*Data)
{
	TextureList[index]->UpdateTexture( Width, Height, Format, DataType, Data);
}