#pragma once

class DeferredRenderBuffers
{

public:
	enum TEXTURE_TYPE
	{
		TEXTURE_TYPE_TEXCOORD,
		TEXTURE_TYPE_POSITION,
		TEXTURE_TYPE_DIFFUSE,
		TEXTURE_TYPE_NORMAL,
		NUM_TEXTURES 

	};

	DeferredRenderBuffers();
	DeferredRenderBuffers(int Width, int Height);

	void StartDeferredRender();
	void BindForGeomPass();
	void BindForStencilPass();
	void BindForLightPass();
	void BindForFinalPass();
	void BindForGeoDepth();
	void CopyDepthForForwardRendering();
	
	void BindForReading(TEXTURE_TYPE TextureType);
	
	void SetReadBuffer(TEXTURE_TYPE TextureType);

protected:
	GLuint		m_Textures[NUM_TEXTURES];
	GLuint		TexDepth;
	GLuint		m_FinalTexture; 
	GLuint		fboID;
};