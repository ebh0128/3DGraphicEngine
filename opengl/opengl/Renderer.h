#pragma once

class Renderer
{
public:
	enum Render_Mode
	{
		FORWARD,
		DEFERRED
	};

	Renderer();
	void Render(SceneGL* Scene);
protected:
	Render_Mode m_Mode;
	DeferredRenderBuffers* GBuffers;
	
};