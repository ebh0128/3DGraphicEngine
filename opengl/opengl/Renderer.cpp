#include "CommonHeader.h"

#include "Camera.h"
#include "Timer.h"

#include "Scene.h"
#include "MyFrameBuffer.h"

#include "AssimpModelNode.h"

 
#include "IOBuffer.h"
#include "DeferredPipeline.h"
#include "Renderer.h"


Renderer::Renderer(SceneGL* Scene)
{
	GLuint W = glutGet(GLUT_WINDOW_WIDTH);
	GLuint H = glutGet(GLUT_WINDOW_HEIGHT);
	GBuffers = new DeferredRenderBuffers(W, H);
	
	m_SSaoBuffer = new IOBuffer();
	m_SSaoBuffer->Init(W, H, false, GL_R32F);
	
	m_BlurBuffer = new IOBuffer();
	m_BlurBuffer->Init(W, H, false, GL_R32F);

	m_pScene = Scene;
	m_pDeferredPipe = new DeferredPipeline(Scene);

	m_Mode = FORWARD;
	m_Mode = DEFERRED;

}
void Renderer::Render()
{
	if (m_Mode == FORWARD) m_pScene->Render();
	else m_pDeferredPipe->DeferredRender();
}
void Renderer::SetScene(SceneGL* Scene)
{
	m_pScene = Scene;
}