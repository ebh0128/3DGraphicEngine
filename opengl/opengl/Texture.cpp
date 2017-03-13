#include "CommonHeader.h"
#include "MyShader.h"
#include "Texture.h"
#include "ProgramManager.h"
#include "ppm.h"


Texture::Texture()
{
	TextureKind = GL_TEXTURE_2D;
	tbo = -1234;
	ShaderLocation[1] = -1000;
}
Texture::Texture(const char* ImagePath, GLuint Channel) :Texture()
{
	CreateTexture(ImagePath, Channel);
}


void Texture::CreateTexture(const char* ImagePath, GLuint Channel)
{
	TextureKind = GL_TEXTURE_2D;
	//채널 설정
	TextureChannel = Channel;
	//Devil Image Load
	ILuint Img;
	ilGenImages(1, &Img);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	ilBindImage(Img);
	ILboolean res = ilLoadImage(ImagePath);
	if (!res)
	{
		printf("ImageLoadFail at %s \n" ,ImagePath);
	}
	//Texture Make

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	glGenTextures(1, &TexID);
	
	glBindTexture(TextureKind, TexID);
	
	m_iWrap[0] = m_iWrap[1] = GL_REPEAT;
	m_iFilter[0] = m_iFilter[1] = GL_LINEAR;
	m_iMipMap = 8;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_iWrap[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_iWrap[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_iFilter[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_iFilter[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_iMipMap);
	glTexImage2D(TextureKind, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH)
		, ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT)
		, ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	//TexID = ilutGLBindTexImage();
	//glGenerateMipmap(TextureKind);


}
//Test Code For Debug
void PrinttoPPM(GLfloat* Data, int Width, int Height)
{
	ppm image(Width, Height);

	for (int i = 0; i < Width*Height; i++)
	{
		double n = Data[i];
		if (n > 1) n = 1;
		image.r[i] = floor(255 * n);
		image.g[i] = floor(255 * n);
		image.b[i] = floor(255 * n);
	}
	image.write("Test.ppm");

}

void Texture::CreateTextureByData(GLfloat* Data, int Width, int Height, GLuint Channel)
{
	//TextureKind = GL_TEXTURE_RECTANGLE;
	TextureKind = GL_TEXTURE_2D;

	TextureChannel = Channel;

	//PrinttoPPM(Data, Width, Height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
	glGenTextures(1, &TexID);

	m_iWrap[0] = m_iWrap[1] = GL_CLAMP_TO_BORDER;
	m_iFilter[0] = m_iFilter[1] = GL_NEAREST;
	m_iMipMap = 0;

	glBindTexture(TextureKind, TexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_iWrap[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_iWrap[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_iFilter[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_iFilter[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_iMipMap);
	glTexImage2D(TextureKind, 0, GL_R32F, Width, Height, 0, GL_RED, GL_FLOAT, Data);
}


void Texture::CreateCubeMapTexture(CubeMapTexturePathInfo* ImagePath, GLuint Channel)
{
	TextureKind = GL_TEXTURE_CUBE_MAP;
	TextureChannel = Channel;

	m_iWrap[0] = m_iWrap[1] = GL_CLAMP_TO_EDGE;
	m_iFilter[0] = m_iFilter[1] = GL_LINEAR;
	m_iMipMap = 0;


	ILuint Img[6];
	ilGenImages(6, Img);

	glGenTextures(1, &TexID);
	glBindTexture(TextureKind, TexID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_iWrap[0]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_iWrap[1]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_iWrap[1]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_iFilter[0]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_iFilter[1]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, m_iMipMap);

	for (int i = 0; i < 6; i++)
	{
		ilBindImage(Img[i]);
		ILboolean res = ilLoadImage(ImagePath->Path[i]);
		if (!res)
		{
			printf("Cube Map [%d] Image Load Fail\n" , i);
		}
		
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH)
			, ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT)
			, ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	
	}

}

void Texture::CreateTextureBuffer(GLfloat* Data, GLuint Size , GLuint VAO, GLuint Format)
{
	TextureFormat = Format;
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo);
	glBufferData(GL_TEXTURE_BUFFER, Size * sizeof(GLfloat), Data, GL_DYNAMIC_DRAW);

	TextureChannel = ProgramManager::GetInstance()->GetChannelID();
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_BUFFER, TexID);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo);
}
GLuint Texture::GetTexture()
{
	return TexID;
}
GLuint Texture::GetTextureChannel()
{
	return TextureChannel;
}
GLuint Texture::GetShaderValue(int i)
{
	return ShaderLocation[i];
}
GLuint  Texture::GetTextureKind()
{
	return TextureKind;
}

// IsDeffered = 1 / 0
void Texture::SetShaderValue(GLuint ShaderLoc, int IsDeferred)
{
	if (IsDeferred > 1) IsDeferred = 1;
	ShaderLocation[IsDeferred] = ShaderLoc;
}

void Texture::ApplyTexture()
{

	//몇번 채널로 텍스쳐 보낼건지
	glActiveTexture(GL_TEXTURE0 + TextureChannel);
	//무슨 텍스쳐 인지
	glBindTexture(TextureKind, TexID);
	glTexParameteri(TextureKind, GL_TEXTURE_WRAP_S, m_iWrap[0]);
	glTexParameteri(TextureKind, GL_TEXTURE_WRAP_T, m_iWrap[1]);
	glTexParameteri(TextureKind, GL_TEXTURE_MAG_FILTER, m_iFilter[0]);
	glTexParameteri(TextureKind, GL_TEXTURE_MIN_FILTER, m_iFilter[1]);
	glTexParameteri(TextureKind, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(TextureKind, GL_TEXTURE_MAX_LEVEL, m_iMipMap);

}

void Texture::UpdateTextureBuffer(GLfloat* Data)
{
	if (tbo < 0) return;
	//glBindBuffer

}
void Texture::ApplyTBO()
{
	if (tbo < 0) return;
	glActiveTexture(GL_TEXTURE0 + TextureChannel);
	glBindTexture(TextureKind, TexID);
	glTexBuffer(TextureKind, GL_RGBA32F, tbo);
}

void Texture::UpdateTexture(GLuint Width, GLuint Height, GLuint Format, GLuint DataType, GLfloat*Data)
{
	glBindTexture(TextureKind, TexID);
	glTexSubImage2D(TextureKind, 0, 0, 0, Width, Height, Format, DataType, Data);
}