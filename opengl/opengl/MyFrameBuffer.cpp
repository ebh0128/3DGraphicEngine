#include "CommonHeader.h"
#include "MyShader.h"
#include "MyFrameBuffer.h"



MyFrameBuffers::MyFrameBuffers()
{
	ColorCount = 0;
}
MyFrameBuffers::MyFrameBuffers(int ColorCnt, int Width, int Height)
{
	ColorCount = ColorCnt;
	

	glGenFramebuffers(1 , &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	pTexColors = new GLuint[ColorCnt];
	
	
	GLenum* pDrawBuffers = new GLenum[ColorCount];

	//Color Texture
	glGenTextures(ColorCount, pTexColors);
	for (int i = 0; i < ColorCount; i++)
	{
		glBindTexture(GL_TEXTURE_2D, pTexColors[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, pTexColors[i], 0);
		pDrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	//Depth Texture;
	glBindTexture(GL_TEXTURE_2D, TexDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TexDepth, 0);

	//프레그먼트에 열어줄 채널들 지정
	glDrawBuffers(ColorCnt, pDrawBuffers);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
	}

	//원래 프레임버퍼로 되돌림 >> 안해주면 큰일남
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	delete[] pDrawBuffers;
}