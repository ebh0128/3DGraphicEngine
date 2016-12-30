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

protected:
	void AddTexture(Texture* pNewtex);
public:
	Sampler();
	Sampler(const char* ImagePath, GLuint Channel, GLint ShaderLoc);
	
	void CreateTexture(const char* ImagePath, GLuint Channel , GLint ShaderLoc);
	
	//1float(R32)
	void CreateTextureByData(GLfloat* Data, int Width, int Height, GLuint Channel, GLint ShaderLoc);
	//���� �ʴ� ä���� ��ȯ
	
	//Path ������ +x -x +y -y +z -z
	void CreateCubeMapTexture(CubeMapTexturePathInfo* ImagePath , GLuint Channel, GLint ShaderLoc);
	void ApplySampler();
	void UpdateTexture(int index,GLuint Width, GLuint Height, GLuint Format, GLuint DataType, GLfloat*Data);
};


