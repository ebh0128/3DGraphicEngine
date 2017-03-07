#pragma once

//���÷� Ŭ������ �������� �ؽ��ĸ� ���� �� �ִ�.
//���� �޽��� ���� ������� ����ȴٸ� �̰� �ϳ��� ����
//���÷��� ��� Uniform1i�� ����ϹǷ� ���� �ؽ����� ���
//GLint �迭�� ����� �ѱ�� �ȴ�.

class Texture;
struct CubeMapTexturePathInfo;

class Sampler
{
protected:
	
	GLuint SamplerID;
	std::vector<Texture*> TextureList;

	//Wrap ����
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
	//���� �ʴ� ä���� ��ȯ
	
	//Path ������ +x -x +y -y +z -z
	void CreateCubeMapTexture(CubeMapTexturePathInfo* ImagePath , GLuint Channel, GLint ShaderLoc);
	void ApplySampler(int isDeferred = 0);
	void UpdateTexture(int index,GLuint Width, GLuint Height, GLuint Format, GLuint DataType, GLfloat*Data);

};


