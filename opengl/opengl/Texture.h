#pragma once


enum TexturePathIndex
{
	POS_X,NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z
};
struct CubeMapTexturePathInfo
{
	const char* Path[6];

};

class Texture
{
protected:
	
	GLuint TexID;

	GLuint tbo;
	// 몇번 텍스쳐로 보낼건지 GL_TEXTUREn
	GLuint TextureChannel;
	GLuint ShaderLocation[2];
	GLuint TextureKind;
	GLuint TextureFormat;
	GLuint m_iWrap[2];
	GLuint m_iFilter[2];
	GLuint m_iMipMap;
	
public:
	Texture();
	Texture(const char* ImagePath, GLuint Channel);
	
	GLuint GetTexture();
	GLuint GetTextureChannel();
	GLuint GetShaderValue(int i);
	GLuint GetTextureKind();

	//glGetUniformLocation 결과값 주기
	void SetShaderValue(GLuint ShaderLoc, int IsDeferred = 0);
	void ApplyTexture();
	
	//일반 텍스쳐 2의 지수승 텍스쳐로 준비할것
	void CreateTexture(const char* ImagePath, GLuint Channel);
	
	//Lookup Table 텍스쳐 사이즈 상관 없음
	//1float texture
	void CreateTextureByData(GLfloat* Data, int Width, int Height, GLuint Channel);
	
	
	
	void CreateCubeMapTexture(CubeMapTexturePathInfo* ImagePath, GLuint Channel);
	//VAO의 경우는 Binding 을 확실히 하기위해
	void CreateTextureBuffer(GLfloat* Data, GLuint Size, GLuint VAO , GLuint Format);

	void UpdateTextureBuffer(GLfloat* Data);
	void ApplyTBO();
	
	void UpdateTexture(GLuint Width, GLuint Height, GLuint Format, GLuint DataType,GLfloat*Data);
	
};