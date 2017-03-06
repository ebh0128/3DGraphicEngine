#include "CommonHeader.h"
#include "MyShader.h"
#include "MyFrameBuffer.h"



DeferredRenderBuffers::DeferredRenderBuffers()
{
	
}
DeferredRenderBuffers::DeferredRenderBuffers(int Width, int Height)
{
	
	glGenFramebuffers(1 , &fboID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);

	glGenTextures(NUM_TEXTURES, m_Textures);
	glGenTextures(1, &TexDepth);
	
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_Textures[i], 0);
	}

	//Depth Texture;
	glBindTexture(GL_TEXTURE_2D, TexDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TexDepth, 0);

	//채널 배열 생성
	GLenum DrawBuffers[] =
	{ 
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
	};

	//프레그먼트에 열어줄 채널들 지정
	glDrawBuffers(NUM_TEXTURES, DrawBuffers);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
	}

	//원래 프레임버퍼로 되돌림 >> 안해주면 큰일남
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

void DeferredRenderBuffers::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);

}
void DeferredRenderBuffers::BindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);
}
void DeferredRenderBuffers::SetReadBuffer(TEXTURE_TYPE TextureType)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}