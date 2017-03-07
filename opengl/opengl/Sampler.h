#pragma once

//샘플러 클래스로 여러개의 텍스쳐를 가질 수 있다.
//같은 메쉬에 같은 방식으로 적용된다면 이거 하나로 가능
//샘플러의 경우 Uniform1i를 사용하므로 여러 텍스쳐의 경우
//GLint 배열을 만들어 넘기면 된다.

class Texture;
struct CubeMapTexturePathInfo;

class Sampler
{
protected:
	
	GLuint SamplerID;
	std::vector<Texture*> TextureList;

	//Wrap 설정
	GLuint m_iWrapS;
	GLuint m_iWrapT;
	GLuint m_iMag;
	GLuint m_iMin;

protected:
	void AddTexture(Texture* pNewtex);
public:
	Sampler();
	Sampler(const char* ImagePath, GLuint Channel, GLint ShaderLoc , GLint DefShaderLoc = -100);
	
	void CreateTexture(const char* ImagePath, GLuint Channel , GLint ShaderLoc, GLint DefShaderLoc = -100);
	
	//1float(R32)
	void CreateTextureByData(GLfloat* Data, int Width, int Height, GLuint Channel, GLint ShaderLoc, GLint DefShaderLoc = -100);
	//쓰지 않는 채널을 반환
	
	//Path 순서는 +x -x +y -y +z -z
	void CreateCubeMapTexture(CubeMapTexturePathInfo* ImagePath , GLuint Channel, GLint ShaderLoc);
	void ApplySampler(int isDeferred = 0);
	void UpdateTexture(int index,GLuint Width, GLuint Height, GLuint Format, GLuint DataType, GLfloat*Data);

};


