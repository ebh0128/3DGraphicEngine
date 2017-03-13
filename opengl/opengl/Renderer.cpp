#include "CommonHeader.h"

#include "Camera.h"
#include "Timer.h"

#include "Scene.h"
#include "MyFrameBuffer.h"

#include "AssimpModelNode.h"

#include "ProgramManager.h"
#include "IOBuffer.h"
#include "Renderer.h"


Renderer::Renderer()
{
	GLuint W = glutGet(GLUT_WINDOW_WIDTH);
	GLuint H = glutGet(GLUT_WINDOW_HEIGHT);
	GBuffers = new DeferredRenderBuffers(W, H);
	
	m_SSaoBuffer = new IOBuffer();
	m_SSaoBuffer->Init(W, H, false, GL_R32F);
	
	m_BlurBuffer = new IOBuffer();
	m_BlurBuffer->Init(W, H, false, GL_R32F);

	m_Mode = FORWARD;
	m_Mode = DEFERRED;
}
void Renderer::Render(SceneGL* Scene)
{
	if (m_Mode == FORWARD) Scene->Render();
	else Scene->DeferredRender(GBuffers , m_SSaoBuffer , m_BlurBuffer);
}
