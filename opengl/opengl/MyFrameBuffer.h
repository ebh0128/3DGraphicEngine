#pragma once

class DeferredRenderBuffers
{

public:
	enum TEXTURE_TYPE
	{
		TEXTURE_TYPE_POSITION,
		TEXTURE_TYPE_DIFFUSE,
		TEXTURE_TYPE_NORMAL,
		TEXTURE_TYPE_TEXCOORD,
		NUM_TEXTURES
	};

	DeferredRenderBuffers();
	DeferredRenderBuffers(int Width, int Height);

	void BindForWriting();
	void BindForReading();
	void SetReadBuffer(TEXTURE_TYPE TextureType);

protected:
	GLuint		m_Textures[NUM_TEXTURES];
	GLuint		TexDepth;
	GLuint		fboID;
};