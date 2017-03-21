#include "CommonHeader.h"

#include "Camera.h"
#include "Scene.h"
#include "Water.h"
#include "LightSystem.h"
#include "DirLight.h"

Water::Water()
{

}
//메쉬 생성은 Create 호출
Water::Water(Object* Parent, SceneGL *Scene, GLfloat MaxH):Object(Parent ,Scene)
{
	m_VertexCount = 0;
	m_TriangleCount = 0;
	m_MaxHeight = MaxH;
	m_DeltaTexCoordS = 0;
	m_DeltaTexCoordT = 0;
	m_RotAngle = 0;
	//쉐이더 초기화 필요
	/*
	GeoShaderName m_pShaderManager->CreateShader(this, "./Shader/Ground_Deferred_GeoPass.vert", "./Shader/Ground_Deferred_GeoPass.frag", "./Shader/Ground_Deferred_GeoPass.tesc",
		"./Shader/Ground_Deferred_GeoPass.tese", "./Shader/Ground_Deferred_GeoPass.geom");
	

	//ShadowShaderName
	*/

	ForwardShaderName = m_pShaderManager->CreateShader(this, "./Shader/Forward_Water.vert"
		, "./Shader/Forward_Water.frag", "./Shader/Forward_Water.tesc"
		, "./Shader/Forward_Water.tese", "./Shader/Forward_Water.geom");

//	ForwardShaderName = m_pShaderManager->CreateShader(this, "./Shader/Test_Water.vert"
//		,"./Shader/Test_Water.frag");
}
void Water::Create(GLuint Xcnt, GLuint Zcnt, GLfloat Offset, GLint TileS, GLint TileT)
{
	Vec*			pVertice;
	Vec*			pTangent;
	Vec*			pNormal;
	IndexVector*	pIndices;
	TexCoord*		pTexcoords;

	m_VertexOffset = Offset;
	m_PatchCountX = Xcnt;
	m_PatchCountZ = Zcnt;

	m_VertexCountX = Xcnt + 1;
	m_VertexCountZ = Zcnt + 1;

	m_VertexCount = m_VertexCountX * m_VertexCountZ;

	m_TileS = TileS;
	m_TileT = TileT;

	pVertice = new Vec[m_VertexCount];
	pNormal = new Vec[m_VertexCount];

	pTexcoords = new TexCoord[m_VertexCount];

	float xStart = -m_VertexOffset * m_PatchCountX / 2;
	float zStart = -m_VertexOffset * m_PatchCountZ / 2;

	for (int i = 0; i < m_VertexCount; i++)
	{
		pNormal[i].x = 0.f; pNormal[i].y = 1.f; pNormal[i].z = 0.f;

		if (i == 65)
		{
			int a = 100;
		}
		//Vertex.x
		pVertice[i].x = xStart + Offset * (i % m_VertexCountX);
		pVertice[i].y = 0;
		//Vertex.z
		pVertice[i].z = zStart + Offset*(i / m_VertexCountZ);
	
		pTexcoords[i].x = (float)1 / Xcnt * (i % m_VertexCountX);
		pTexcoords[i].y = (float)1 / Zcnt * (i / m_VertexCountZ);
	}
	m_TriangleCount = m_PatchCountX * m_PatchCountZ * 2;
	pIndices = new InVec[m_TriangleCount];

	int xindex = 0;
	int zindex = 0;
	int j = 0;
	int facecnt = 0;

	//맨 마지막줄은 버텍스가 생성 되지 않아야됨
	while (j < m_VertexCountX * (m_VertexCountZ - 1))
	{
		if (xindex == m_PatchCountX)
		{
			xindex = 0;
			zindex++;
			j++;
			continue;
		}
		pIndices[facecnt].A = j;
		pIndices[facecnt].B = j + m_VertexCountX;
		pIndices[facecnt].C = j + 1;
		facecnt++;

		pIndices[facecnt].A = j + m_VertexCountX;
		pIndices[facecnt].B = j + m_VertexCountX + 1;
		pIndices[facecnt].C = j + 1;
		facecnt++;

		xindex++;
		j++;
	}

	pTangent = new Vec[m_VertexCount];
	//탄젠트 만들기
	for (int i = 0; i < m_TriangleCount; i++)
	{
		int index0 = pIndices[i].A;
		int index1 = pIndices[i].B;
		int index2 = pIndices[i].C;

		glm::vec3 Vert0 = glm::vec3(pVertice[index0].x, pVertice[index0].y, pVertice[index0].z);
		glm::vec3 Vert1 = glm::vec3(pVertice[index1].x, pVertice[index1].y, pVertice[index1].z);
		glm::vec3 Vert2 = glm::vec3(pVertice[index2].x, pVertice[index2].y, pVertice[index2].z);

		glm::vec2 Tex0 = glm::vec2(pTexcoords[index0].x, pTexcoords[index0].y);
		glm::vec2 Tex1 = glm::vec2(pTexcoords[index1].x, pTexcoords[index1].y);
		glm::vec2 Tex2 = glm::vec2(pTexcoords[index2].x, pTexcoords[index2].y);

		glm::vec3 Edge1 = Vert1 - Vert0;
		glm::vec3 Edge2 = Vert2 - Vert0;
		
		float DeltaU1 = Tex1.x - Tex0.x;
		float DeltaV1 = Tex1.y - Tex0.y;
		float DeltaU2 = Tex2.x - Tex0.x;
		float DeltaV2 = Tex2.y - Tex0.y;

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		pTangent[index0].x = f*(DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
		pTangent[index0].y = f*(DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
		pTangent[index0].z = f*(DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);
	
		pTangent[index1] = pTangent[index2] = pTangent[index0];
		int a = 10;
	}

	MeshEntry* WaterMesh = new MeshEntry((GLfloat*)pVertice, m_VertexCount * 3, (GLuint*)pIndices, m_TriangleCount * 3, (GLfloat*)pNormal,
		(GLfloat*)pTexcoords, m_VertexCount * 2 , (GLfloat*)pTangent);

	m_DiffuseTexUnit = 11;
	m_NormalTexUnit = 10;
	m_DisplaceTexUnit =9;

	Texture* DiffuseTex = new Texture("./Texture/water_Diffuse.jpg", m_DiffuseTexUnit);
	WaterMesh->AddTexture(DiffuseTex);
	Texture* NormalTex = new Texture("./Texture/water_Normal.jpg", m_NormalTexUnit);
	WaterMesh->AddTexture(NormalTex);
	Texture* DisplaceMap = new Texture("./Texture/water_Disp.jpg", m_DisplaceTexUnit);
	WaterMesh->AddTexture(DisplaceMap);

	int strSize = sizeof(PaddingLight);
	AddUBO(nullptr, strSize*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0, m_pShaderManager->ApplyShaderByName(ForwardShaderName));

	m_pModel = new Model();
	m_pModel->AddMesh(WaterMesh);
	m_pModel->MakeInstancingBuffer();

	m_pModel->SetPrimitiveKind(GL_PATCHES);


	delete[] pVertice;
	delete[] pIndices;
	delete[] pNormal;
	delete[] pTexcoords;
	delete[] pTangent;

}


void Water::ShaderParamInit(MyShader* ManagedShader)
{

	MyShader* ThisShader = m_pShaderManager->ApplyShaderByName(ForwardShaderName);
	ThisShader->SetUniform1i("TextureDiffuse", m_DiffuseTexUnit);
	ThisShader->SetUniform1i("TextureNormal", m_NormalTexUnit);
	ThisShader->SetUniform1i("TextureDisplacement", m_DisplaceTexUnit);
	
	ThisShader->SetUniform1f("TileS", m_TileS);
	ThisShader->SetUniform1f("TileT", m_TileT);
	ThisShader->SetUniform1f("gTessLevel", 32.0);
	
	ThisShader->SetUniform1f("gDeltaS", m_DeltaTexCoordS);
	ThisShader->SetUniform1f("gDeltaT", m_DeltaTexCoordT);
	ThisShader->SetUniform1f("gDispFactor", m_MaxHeight);

	ThisShader->SetUniform1i("ShadowMap", 8);

	glm::mat4 LightSpaceMat = pScene->GetDirectionalLight()->GetLightVPMat();

	ThisShader->SetUniformMatrix4fv("lightSpaceMat", glm::value_ptr(LightSpaceMat));

	Object::ShaderParamInit(ThisShader);
}
void Water::Render()
{
	Object::Render();
}
void Water::Update(GLfloat dtime)
{
	GLfloat RotSpeed = 15;
	m_RotAngle += 3.141592*0.2*dtime;//glm::radians(15 * dtime);
	m_DeltaTexCoordS = 0;
	
	m_DeltaTexCoordT += 0.1*dtime;
	
}