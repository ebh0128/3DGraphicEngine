#include "CommonHeader.h"
#include "Camera.h"
#include "Scene.h"
#include "PassShaderObject.h"
#include "DeferredPipeLine.h"


DeferredPipeline::DeferredPipeline(SceneGL* pScene)
{
	m_pScene = pScene;
	m_pShaderManager = pScene->GetShaderManager();
	MakePipeLine();
}

void DeferredPipeline::MakePipeLine()
{
	SortedPipeLineObject[SHADOW_GROUND_PASS] = m_pShaderManager->FindPassShader("/Shader/Shadow_Ground");
	SortedPipeLineObject[SHADOW_GOBJECT_PASS] = m_pShaderManager->FindPassShader("/Shader/Shadow_InstanceObj");

	SortedPipeLineObject[GEO_GROUND_PASS] = m_pShaderManager->FindPassShader("/Shader/Ground_Deferred_GeoPass");
	SortedPipeLineObject[GEO_OBJECT_PASS] = m_pShaderManager->FindPassShader("/Shader/Deferred_GeoPass");

	SortedPipeLineObject[SSAO_PASS] = m_pShaderManager->FindPassShader("/Shader/SSAO");
	SortedPipeLineObject[BLUR_PASS] = m_pShaderManager->FindPassShader("/Shader/Blur");

	SortedPipeLineObject[POINT_STENCIL_PASS] = m_pShaderManager->FindPassShader("/Shader/nullShader");
	SortedPipeLineObject[POINT_LIGHT_PASS] = m_pShaderManager->FindPassShader("/Shader/Deferred_PointLight");

	SortedPipeLineObject[DIR_LIGHT_PASS] = m_pShaderManager->FindPassShader("/Shader/Deferred_DirLight");

	SortedPipeLineObject[FORWARD_SKY_PASS] = m_pShaderManager->FindPassShader("SkyBox");
	SortedPipeLineObject[FORWARD_POINT_PASS] = m_pShaderManager->FindPassShader("PointLight");


	SortedPipeLineObject[HDR_PASS] = m_pShaderManager->FindPassShader("/Shader/HDR");


}

void DeferredPipeline::SortShaderObject()
{

}