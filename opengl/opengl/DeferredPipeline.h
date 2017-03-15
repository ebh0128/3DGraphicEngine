#pragma once

class DeferredPipeline
{
protected:
	
	enum PipelineOrder
	{
		SHADOW_GROUND_PASS,
		SHADOW_GOBJECT_PASS,

		GEO_GROUND_PASS,
		GEO_OBJECT_PASS,

		SSAO_PASS,
		BLUR_PASS,

		POINT_STENCIL_PASS,
		POINT_LIGHT_PASS,

		DIR_LIGHT_PASS,

		FORWARD_SKY_PASS,
		FORWARD_POINT_PASS,

		HDR_PASS,
		PASS_MAX

	};
	
	ShaderManager * m_pShaderManager;

	SceneGL * m_pScene;
	PassShaderObject* SortedPipeLineObject[PASS_MAX];


	void SortShaderObject();

public:
	DeferredPipeline(SceneGL* pScene);

	void MakePipeLine();

};