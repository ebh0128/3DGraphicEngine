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
	// ��� �ؽ��ķ� �������� GL_TEXTUREn
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

	//glGetUniformLocation ����� �ֱ�
	void SetShaderValue(GLuint ShaderLoc, int IsDeferred = 0);
	void ApplyTexture();
	
	//�Ϲ� �ؽ��� 2�� ������ �ؽ��ķ� �غ��Ұ�
	void CreateTexture(const char* ImagePath, GLuint Channel);
	
	//Lookup Table �ؽ��� ������ ��� ����
	//1float texture
	void CreateTextureByData(GLfloat* Data, int Width, int Height, GLuint Channel);
	
	
	
	void CreateCubeMapTexture(CubeMapTexturePathInfo* ImagePath, GLuint Channel);
	//VAO�� ���� Binding �� Ȯ���� �ϱ�����
	void CreateTextureBuffer(GLfloat* Data, GLuint Size, GLuint VAO , GLuint Format);

	void UpdateTextureBuffer(GLfloat* Data);
	void ApplyTBO();
	
	void UpdateTexture(GLuint Width, GLuint Height, GLuint Format, GLuint DataType,GLfloat*Data);
	
};