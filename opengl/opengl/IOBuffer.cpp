#include "CommonHeader.h"
#include "MyShader.h"
#include "IOBuffer.h"


IOBuffer::IOBuffer()
{
	m_fbo = 0;
	m_texture = 0;
	m_Depth = 0;
	m_internalType = GL_NONE;
}
IOBuffer::IOBuffer(GLuint Width, GLuint Height, bool WithDepth, GLenum InternalType)
{
	Init(Width, Height, WithDepth, InternalType);
}

bool IOBuffer::Init(GLuint Width, GLuint Height, bool WithDepth, GLenum InternalType)
{
	m_internalType = InternalType;

	GLenum Format, Type;

	switch (InternalType)
	{
	case GL_RGB32F:
		Format = GL_RGB;
		Type = GL_FLOAT;
		break;
	case GL_R32F:
		Format = GL_RED;
		Type = GL_FLOAT;
		break;
	case GL_NONE:
		break;
	}
	
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	if (InternalType != GL_NONE)
	{
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, InternalType, Width, Height, 0, Format, Type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);

	}


	// Create the depth buffer 
	if (WithDepth) {
		glGenTextures(1, &m_Depth);

		// depth
		glBindTexture(GL_TEXTURE_2D, m_Depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_Depth, 0);
	}

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FB error , status : 0x%x\n", Status);
		return false;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	return true;
}
void IOBuffer::BindForWriting()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}
void IOBuffer::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);

	if (m_internalType == GL_NONE) {
		glBindTexture(GL_TEXTURE_2D, m_Depth);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}

}
