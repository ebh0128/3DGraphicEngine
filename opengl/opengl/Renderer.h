#pragma once

class Renderer
{
public:
	enum Render_Mode
	{
		FORWARD,
		DEFERRED
	};

	Renderer(SceneGL* Scene);

	void SetScene(SceneGL* Scene);
	void Render();
protected:
	
	DeferredPipeline* m_pDeferredPipe;
	SceneGL* m_pScene;
	Render_Mode m_Mode;
	DeferredRenderBuffers* GBuffers;
	IOBuffer* m_SSaoBuffer;
	IOBuffer* m_BlurBuffer;
};