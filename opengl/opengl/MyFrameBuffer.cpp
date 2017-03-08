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
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_Textures[i], 0);
	}

	//Depth Texture;
	glBindTexture(GL_TEXTURE_2D, TexDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, Width, Height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, TexDepth, 0);

	//���� ��� �ؽ���

	glBindTexture(GL_TEXTURE_2D, m_FinalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_FinalTexture, 0);


	//ä�� �迭 ����
	/*
	GLenum DrawBuffers[] =
	{ 
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
	};

	//�����׸�Ʈ�� ������ ä�ε� ����
	glDrawBuffers(NUM_TEXTURES, DrawBuffers);
	*/
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
	}

	//���� �����ӹ��۷� �ǵ��� >> �����ָ� ū�ϳ�
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

void DeferredRenderBuffers::StartDeferredRender()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
	//������ �ؽ��� �ʱ�ȭ
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	glClear(GL_COLOR_BUFFER_BIT);
}
void DeferredRenderBuffers::BindForStencilPass()
{
	glDrawBuffer(GL_NONE);
}
void DeferredRenderBuffers::BindForFinalPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);
	glReadBuffer(GL_COLOR_ATTACHMENT4);
}
void DeferredRenderBuffers::BindForGeomPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
	GLenum DrawBuffers[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
	};

	//�����׸�Ʈ�� ������ ä�ε� ����
	glDrawBuffers(NUM_TEXTURES, DrawBuffers);



}
void DeferredRenderBuffers::BindForLightPass()
{
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	for (unsigned int i = 0; i < 4; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
	}
}
void DeferredRenderBuffers::SetReadBuffer(TEXTURE_TYPE TextureType)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}