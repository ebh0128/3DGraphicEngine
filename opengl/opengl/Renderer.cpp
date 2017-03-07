#include "CommonHeader.h"

#include "Camera.h"
#include "myShader.h"
#include "Timer.h"
#include "Mesh.h"
#include "Scene.h"
#include "MyFrameBuffer.h"
#include "AssimpModelNode.h"
#include "Object.h"
#include "ProgramManager.h"
#include "Renderer.h"


Renderer::Renderer()
{
	GBuffers = new DeferredRenderBuffers(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	m_Mode = FORWARD;
	//m_Mode = DEFERRED;
}
void Renderer::Render(SceneGL* Scene)
{
	if (m_Mode == FORWARD) Scene->Render();
	else Scene->DeferredRender(GBuffers);
}
