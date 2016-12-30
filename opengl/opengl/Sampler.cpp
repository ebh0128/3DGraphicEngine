#pragma once
#include "CommonHeader.h"
#include "MyShader.h"
#include "Texture.h"
#include "Sampler.h"


Sampler::Sampler()
{
	glGenSamplers(1, &SamplerID);
	glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
Sampler::Sampler(const char* ImagePath, GLuint Channel, GLint ShaderLoc) :Sampler()
{

	CreateTexture(ImagePath, Channel, ShaderLoc);
}

void Sampler::CreateTexture(const char* ImagePath, GLuint Channel , GLint ShaderLoc)
{
	
	Texture* pNew = new Texture(ImagePath, Channel);
	pNew->SetShaderValue(ShaderLoc);
	AddTexture(pNew);
	glBindSampler(Channel, SamplerID);
}
void Sampler::CreateTextureByData(GLfloat* Data, int Width, int Height, GLuint Channel, GLint ShaderLoc)
{

	glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glSamplerParameteri(SamplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(SamplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	Texture* pNew = new Texture();
	pNew->CreateTextureByData(Data, Width, Height, Channel);
	pNew->SetShaderValue(ShaderLoc);
	AddTexture(pNew);
	glBindSampler(Channel, SamplerID);
}


void Sampler::CreateCubeMapTexture(CubeMapTexturePathInfo* ImagePath, GLuint Channel, GLint ShaderLoc)
{
	glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
void Sampler::ApplySampler()
{
	for (int i = 0; i < TextureList.size(); i++)
	{
		TextureList[i]->ApplyTexture();


	}
}
void Sampler::UpdateTexture(int index, GLuint Width, GLuint Height, GLuint Format, GLuint DataType, GLfloat*Data)
{
	TextureList[index]->UpdateTexture( Width, Height, Format, DataType, Data);
}