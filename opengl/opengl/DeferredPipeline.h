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
	
	DeferredRenderBuffers* m_pGbuffer;
	IOBuffer* m_pSSAOBuffer;
	IOBuffer* m_pBlurBuffer;
	IOBuffer* m_pShadowBuffer;

	void SortShaderObject();

public:
	DeferredPipeline(SceneGL* pScene);

	void MakePipeLine();

	void DeferredRender();

	//그림자 패스
	void ShadowMapPass();

	//지오메트리 패스
	void RenderGeoPass();
	
	//SSAO
	void SSAOPass();
	void BlurPass();

	//포인트 라이트
	void RenderStencilPass();
	void RenderPointLitPass();
	
	//Dir 라이트
	void RenderDirLitPass();

	void RenderForwardObjPass();
	
	//최종결과(HDR)
	void RenderFinalPass();

	void PassRender(PassShaderObject* PassObj , void (DeferredPipeline::*InitMethod)(MyShader*,Object*));


	

private:
	//각 쉐이더들 초기화 함수
	void ShadowObjectInit(MyShader* sha, Object* Obj);
	
	void GeoGroundInit(MyShader* sha, Object* Obj);
	void GeoObjectInit(MyShader* sha, Object* Obj);

	void SSAOInit(MyShader* sha, Object* Obj);
	void BlurInit(MyShader* sha, Object* Obj);

	void PointStencilInit(MyShader* sha, Object* Obj);
	void PointLightInit(MyShader* sha, Object* Obj);

	void DirLightInit(MyShader* sha, Object* Obj);

	void ForwardSkyInit(MyShader* sha, Object* Obj);
	void ForwardPointInit(MyShader* sha, Object* Obj);

	void HDRInit(MyShader* sha, Object* Obj);

};