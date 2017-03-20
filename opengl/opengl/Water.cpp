#include "CommonHeader.h"

#include "Camera.h"
#include "Scene.h"
#include "Water.h"



Water::Water()
{

}
//메쉬 생성은 Create 호출
Water::Water(Object* Parent, SceneGL *Scene, GLfloat MaxH):Object(Parent ,Scene)
{
	m_VertexCount = 0;
	m_TriangleCount = 0;

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

	MeshEntry* WaterMesh = new MeshEntry((GLfloat*)pVertice, m_VertexCount * 3, (GLuint*)pIndices, m_TriangleCount * 3, (GLfloat*)pNormal,
		(GLfloat*)pTexcoords, m_VertexCount * 2);

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
	ThisShader->SetUniform1f("gDispFactor", m_MaxHeight);

	Object::ShaderParamInit(ThisShader);
}
void Water::Render()
{
	Object::Render();
}