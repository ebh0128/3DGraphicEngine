#include "CommonHeader.h"
#include "Camera.h"
#include "Scene.h"
#include "MyFrameBuffer.h"
#include "IOBuffer.h"
#include "PassShaderObject.h"
#include "DeferredPipeLine.h"

#include "DirLight.h"
#include "PatchedGround.h"
#include "AssimpModelNode.h"
#include "LightSystem.h"



DeferredPipeline::DeferredPipeline(SceneGL* pScene)
{
	m_pScene = pScene;
	m_pShaderManager = pScene->GetShaderManager();
	MakePipeLine();


	//버퍼들 초기화
	GLuint W = glutGet(GLUT_WINDOW_WIDTH);
	GLuint H = glutGet(GLUT_WINDOW_HEIGHT);
	m_pGbuffer = new DeferredRenderBuffers(W, H);

	m_pSSAOBuffer = new IOBuffer();
	m_pSSAOBuffer->Init(W, H, false, GL_R32F);

	m_pBlurBuffer = new IOBuffer();
	m_pBlurBuffer->Init(W, H, false, GL_R32F);

	m_pShadowBuffer = new IOBuffer();
	m_pShadowBuffer->Init(1024, 1024, true, GL_NONE);
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


void DeferredPipeline::PassRender(PassShaderObject* PassObj, void(DeferredPipeline::*InitMethod)(MyShader*, Object*))
{
	MyShader* CurruntPassShader;
	CurruntPassShader = PassObj->ApplyPassShader();
	for (int i = 0; i < PassObj->GetPassObjectCount(); i++)
	{
		Object* DrawObj = PassObj->GetDrawObject(i);
		(this->*InitMethod)(CurruntPassShader, DrawObj);
		DrawObj->RenderByPipeLine();
	}
}



void DeferredPipeline::DeferredRender()
{
	ShadowMapPass();

	m_pGbuffer->StartDeferredRender();
	glClearColor(0, 0, 0, 0);

	RenderGeoPass();

	SSAOPass();
	BlurPass();

	//SSAO패스 초기화
	m_pGbuffer->BindForGeomPass();

	/////////스탠실 만들기

	glEnable(GL_STENCIL_TEST);
	RenderStencilPass();

	/////////Light 패스 시작
	RenderPointLitPass();

	glDisable(GL_STENCIL_TEST);
	
	RenderDirLitPass();

	//Forward Rendering//////////////////////////
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	RenderForwardObjPass();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	//현재 출력 버퍼 HDR임
	//결과 출력
	RenderFinalPass();


}

//그림자 패스
void DeferredPipeline::ShadowMapPass()
{
	glViewport(0, 0, 1024, 1024);
	m_pShadowBuffer->BindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	//지오메트리에서 플랜드 필요없음
	glDisable(GL_BLEND);

	//쉐이더 적용
	//쉐이더 변수 전송
	//Object Draw

	
	//지형Shadow 패스
	PassRender(SortedPipeLineObject[SHADOW_GROUND_PASS], (&DeferredPipeline::ShadowObjectInit));

	//오브젝트 Shadow 패스 (그림자 초기화는 동일)
	PassRender(SortedPipeLineObject[SHADOW_GOBJECT_PASS], (&DeferredPipeline::ShadowObjectInit));
	

	glViewport(0, 0, 1024, 768);

}
//지오메트리 패스
void DeferredPipeline::RenderGeoPass()
{
	m_pGbuffer->BindForGeomPass();

	//깊이 버퍼는 지오메트리 에서만
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	//지오메트리에서 플랜드 필요없음
	glDisable(GL_BLEND);

	//쉐이더 적용
	//쉐이더 변수 전송
	//Object Draw

	//지형 Geo 패스
	PassRender(SortedPipeLineObject[GEO_GROUND_PASS], &DeferredPipeline::GeoGroundInit);
	
	//오브젝트 Geo패스
	PassRender(SortedPipeLineObject[GEO_OBJECT_PASS], &DeferredPipeline::GeoObjectInit);

	
//	Root->RenderGeoPass();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

}

//SSAO
void DeferredPipeline::SSAOPass()
{
	m_pGbuffer->BindForReading(DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	m_pGbuffer->BindForReading(DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);

	m_pSSAOBuffer->BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT);

	//쉐이더 적용
	//쉐이더 변수 전송
	//Object Draw

	//SSAO Pass
	PassRender(SortedPipeLineObject[SSAO_PASS], &DeferredPipeline::SSAOInit);


	//m_pDirLight->SSAOPass();
}
void DeferredPipeline::BlurPass()
{
	m_pSSAOBuffer->BindForReading(GL_TEXTURE0);


	m_pBlurBuffer->BindForWriting();

	glClear(GL_COLOR_BUFFER_BIT);

	//쉐이더 적용
	//쉐이더 변수 전송
	//Object Draw

	PassRender(SortedPipeLineObject[BLUR_PASS], &DeferredPipeline::BlurInit);

	//m_pDirLight->BlurPass();
}

//포인트 라이트
void DeferredPipeline::RenderStencilPass()
{
	m_pGbuffer->BindForStencilPass();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	//쉐이더 적용
	//쉐이더 변수 전송
	//Object Draw

	PassRender(SortedPipeLineObject[POINT_STENCIL_PASS], &DeferredPipeline::PointStencilInit);


//	m_pPointLightSys->RenderStencilPass();

}
void DeferredPipeline::RenderPointLitPass()
{

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	m_pGbuffer->BindForLightPass();
	glClear(GL_COLOR_BUFFER_BIT);

	//쉐이더 적용
	//쉐이더 변수 전송
	//Object Draw

	PassRender(SortedPipeLineObject[POINT_LIGHT_PASS], &DeferredPipeline::PointLightInit);


	//m_pPointLightSys->RenderPointLitPass();

	glCullFace(GL_BACK);
	glDisable(GL_BLEND);

}

//Dir 라이트
void DeferredPipeline::RenderDirLitPass()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	//대략 잡은거
	m_pBlurBuffer->BindForReading(GL_TEXTURE7);
	m_pShadowBuffer->BindForReading(GL_TEXTURE8);

	m_pGbuffer->BindForLightPass();

	PassRender(SortedPipeLineObject[DIR_LIGHT_PASS], &DeferredPipeline::DirLightInit);

	//m_pDirLight->RenderDirLitPass();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

}

void DeferredPipeline::RenderForwardObjPass()
{
	//스카이 박스 (뎁스 리드)
	PassRender(SortedPipeLineObject[FORWARD_SKY_PASS], &DeferredPipeline::ForwardSkyInit);
	//포인트 라이트
	PassRender(SortedPipeLineObject[FORWARD_POINT_PASS], &DeferredPipeline::ForwardPointInit);
}

//최종결과(HDR)
void DeferredPipeline::RenderFinalPass()
{
	GLsizei W = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei H = glutGet(GLUT_WINDOW_HEIGHT);

	m_pGbuffer->BindForFinalPass();
	//그냥 그리기
	//glBlitFramebuffer(0, 0, W, H, 0, 0, W, H, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	PassRender(SortedPipeLineObject[HDR_PASS], &DeferredPipeline::HDRInit);

	//HDR + reinhard tone mapping
	//m_pDirLight->HDRPass();

	//이 다음부터 Forward 해야되므로 초기화
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

}








///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////단계별 쉐이더 초기화///////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void DeferredPipeline::ShadowObjectInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	glm::mat4 LightSpaceMat = m_pScene->GetDirectionalLight()->GetLightVPMat();
	glm::mat4 M;

	M = Obj->GetModelMat();

	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("lightSpaceMat", glm::value_ptr(LightSpaceMat));

}
void DeferredPipeline::GeoGroundInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;
	glm::mat4 V = m_pScene->GetVMatrix();
	glm::mat4 VP = m_pScene->GetVPMatrix();
	glm::mat4 M;

	PatchedGround* pGround = (PatchedGround*)Obj;

	M = pGround->GetModelMat();

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;

	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("V", glm::value_ptr(V));
	ThisShader->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	ThisShader->SetUniform1i("TextureGround", pGround->GetMainTexUnit());
	ThisShader->SetUniform1i("TextureSnow", pGround->GetSnowTexUnit());
	ThisShader->SetUniform1i("TextureStone", pGround->GetStoneTexUnit());
	ThisShader->SetUniform1i("SamplerNoise", pGround->GetNoiseTexUnit());

}
void DeferredPipeline::GeoObjectInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	glm::mat4 V = m_pScene->GetVMatrix();
	glm::mat4 VP = m_pScene->GetVPMatrix();
	glm::mat4 M;

	AssimpObject* pAssimp = (AssimpObject*)Obj;
	
	M = pAssimp->GetModelMat();

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;
	ThisShader->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
	ThisShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	ThisShader->SetUniformMatrix4fv("V", glm::value_ptr(V));
	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	ThisShader->SetUniform1i("IsTextured", pAssimp->GetIsTextured() ? 1 : 0);
	ThisShader->SetUniform1i("TextureMain", pAssimp->GetMainTexUint());
}
void DeferredPipeline::SSAOInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	ThisShader->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	ThisShader->SetUniform1i("gNormalMap", DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);

	glm::mat4 P = m_pScene->GetPMatrix();

	DirLight* DLight =  (DirLight*)Obj;
	//NoiseTexture 전송
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, DLight->GetNoiseTexture());
	ThisShader->SetUniform1i("gNoiseTexture", 10);

	ThisShader->SetUniformMatrix4fv("P", glm::value_ptr(P));
	ThisShader->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	ThisShader->SetUniform1f("gSampleRad", 0.3f);
	float* Kernel = DLight->GetKernel();
	ThisShader->SetUniform3fv("gKernel", (GLfloat*)Kernel, KERNEL_SIZE);

}
void DeferredPipeline::BlurInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	ThisShader->SetUniform1i("gColorMap", 0);


}
void DeferredPipeline::PointStencilInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	glm::mat4 M = Obj->GetModelMat();
	glm::mat4 VP = m_pScene->GetVPMatrix();
	glm::mat4 MVP = VP*M;

	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));

}
void DeferredPipeline::PointLightInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	LightSystem* PtLightSys = (LightSystem*)Obj;

	glm::mat4 M = PtLightSys->GetModelMat();
	glm::mat4 V = m_pScene->GetVMatrix();
	glm::mat4 VP = m_pScene->GetVPMatrix();
	glm::mat4 MVP = VP*M;
	
	ThisShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	ThisShader->SetUniformMatrix4fv("V", glm::value_ptr(V));

	//안씀
	//glm::mat4 ParentsTransform = InstanceList[0]->GetParentsMat();
	//ThisShader->SetUniformMatrix4fv("LightTransMat", glm::value_ptr(ParentsTransform));


	glm::vec2 ScreenSize = glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	ThisShader->SetUniform2fv("gScreenSize", glm::value_ptr(ScreenSize));

	glm::vec4 CameraPos = m_pScene->GetCurrentCamPos();
	ThisShader->SetUniform4fv("gEyeWorldPos", glm::value_ptr(CameraPos));

	//gBuffer 텍스쳐 보내기
	ThisShader->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	ThisShader->SetUniform1i("gColorMap", DeferredRenderBuffers::TEXTURE_TYPE_DIFFUSE);
	ThisShader->SetUniform1i("gNormalMap", DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);

	LightList* DataforShader = m_pScene->GetLightSrouceArray();
	GLuint Size = DataforShader->Count * sizeof(PaddingLight);
	//meshes[i]->UpdateUBO(DataforShader, Size+ sizeof(GLuint), 0);
	PtLightSys->UpdateUBO(DataforShader, sizeof(GLuint), 0);

	// std140 stride 16
	PtLightSys->UpdateUBO(DataforShader, Size, 12);
	//	UpdateUBO(DataforShader, Size, 16);

	PtLightSys->ScaleLightforDeferred();

}
void DeferredPipeline::DirLightInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	DirLight* DLight = (DirLight*)Obj;

	glm::vec4 Diffuse =glm::vec4( DLight->GetDif(),1);
	ThisShader->SetUniform4fv("DiffuseCol", glm::value_ptr(Diffuse));

	glm::mat4 M = DLight->GetModelMat();
	glm::mat4 VP = m_pScene->GetVPMatrix();
	glm::mat4 MVP = VP*DLight->GetModelMat();
	glm::mat4 V = m_pScene->GetVMatrix();
	ThisShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));
	ThisShader->SetUniformMatrix4fv("V", glm::value_ptr(V));

	glm::mat4 InvV = glm::inverse(V);
	ThisShader->SetUniformMatrix4fv("InverseV", glm::value_ptr(InvV));


	glm::vec2 ScreenSize = glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	ThisShader->SetUniform2fv("gScreenSize", glm::value_ptr(ScreenSize));

	glm::vec4 CameraPos = m_pScene->GetCurrentCamPos();
	//
	ThisShader->SetUniform4fv("gEyeWorldPos", glm::value_ptr(CameraPos));

	//Light 정보 보내기
	glm::vec4 LightPos = DLight->GetPos();
	ThisShader->SetUniform4fv("gDirLight.LPos", glm::value_ptr(LightPos));

	glm::vec4 paramDiff = glm::vec4(DLight->GetDif(), 1);
	glm::vec4 paramAmbi = glm::vec4(DLight->GetAmb(), 1);
	glm::vec4 paramSpec = glm::vec4(DLight->GetSpec(), 1);

	ThisShader->SetUniform4fv("gDirLight.LDiff", glm::value_ptr(paramDiff));
	ThisShader->SetUniform4fv("gDirLight.LAmbi", glm::value_ptr(paramAmbi));
	ThisShader->SetUniform4fv("gDirLight.LSpec", glm::value_ptr(paramSpec));


	//gBuffer 텍스쳐 보내기
	ThisShader->SetUniform1i("gPositionMap", DeferredRenderBuffers::TEXTURE_TYPE_POSITION);
	ThisShader->SetUniform1i("gColorMap", DeferredRenderBuffers::TEXTURE_TYPE_DIFFUSE);
	ThisShader->SetUniform1i("gNormalMap", DeferredRenderBuffers::TEXTURE_TYPE_NORMAL);
	ThisShader->SetUniform1i("AOMap", 7);

	ThisShader->SetUniform1i("ShadowMap", 8);

	glm::mat4 LightSpaceMat = m_pScene->GetDirectionalLight()->GetLightVPMat();

	ThisShader->SetUniformMatrix4fv("lightSpaceMat", glm::value_ptr(LightSpaceMat));


}
void DeferredPipeline::ForwardSkyInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	if (!ThisShader) return;


	GLint getDepth = 123;
	glGetIntegerv(GL_DEPTH_WRITEMASK, &getDepth);
	glm::mat4 V = m_pScene->GetVMatrix();
	//카메라 이동은 무시
	V[3][0] = 0; V[3][1] = 0; V[3][2] = 0;
	glm::mat4 P = m_pScene->GetPMatrix();
	glm::mat4 VP = P*V;
	glm::mat4 invV = glm::inverse(V);
	glm::mat4 invP = glm::inverse(P);
	glm::mat4 InversVP = glm::inverse(VP);
	//glm::mat4 InversVP = invP*invV;
	//InversVP = InversVP / InversVP[3][3];
	ThisShader->SetUniformMatrix4fv("InversVP", glm::value_ptr(InversVP));

	glm::vec4 LightDiffuse = glm::vec4(m_pScene->GetDirectionalLight()->GetDif(), 1);
	ThisShader->SetUniform4fv("DiffuseCol", glm::value_ptr(LightDiffuse));
	ThisShader->SetUniform1i("SkyBoxTexture", Obj->GetMainTextureUnit());
	

}
void DeferredPipeline::ForwardPointInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	glm::vec4 TempDiff = glm::vec4(1, 1, 1, 1);
	ThisShader->SetUniform4fv("DiffuseCol", glm::value_ptr(TempDiff));

	glm::mat4 VP = m_pScene->GetVPMatrix();
	glm::mat4 MVP = VP*Obj->GetModelMat();
	ThisShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));

	//광원 그리기용 스케일
	glm::vec3 PtScale = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::mat4 NewTrans = Obj->GetModelMat()*glm::scale(PtScale);
	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(NewTrans));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));

}
void DeferredPipeline::HDRInit(MyShader* sha, Object* Obj)
{
	MyShader* ThisShader;
	if (sha == nullptr) return;
	else ThisShader = sha;

	ThisShader->SetUniform1i("gFinalMap", 4);
	ThisShader->SetUniform1f("exposure", 0.7);

}