#pragma once


class IOBuffer
{
public:

	IOBuffer();
	IOBuffer(GLuint Width, GLuint Height, bool WithDepth, GLenum InternalType);
	
	bool Init(GLuint Width, GLuint Height, bool WithDepth, GLenum InternalType);
	void BindForWriting();
	void BindForReading(GLenum TextureUnit);

private:
	GLuint m_fbo;
	GLuint m_texture;
	GLuint m_Depth;
	GLuint m_internalType;

};