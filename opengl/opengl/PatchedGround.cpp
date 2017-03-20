#pragma once
#include "CommonHeader.h"
#include "Camera.h"


#include "Scene.h"
#include "PerlinNoise.h"
 

#include "PatchedGround.h"

#include "Texture.h"
#include "DirLight.h"

PatchedGround::PatchedGround()
{

}
PatchedGround::PatchedGround(Object* Parent, SceneGL *Scene, int seed, GLfloat MaxH) :Object(Parent, Scene)
{
	VertexCount = 0;
	TriangleCount = 0;
	NoiseSeed = seed;
	Perlin = new PerlinNoise(seed);
	MaxHeight = MaxH;

	//pShader = new MyShader();
	//pShader->build_program_from_files("BasicShader.vert", "BasicShader.frag");
//	ForwardShaderName = m_pShaderManager->CreateShader(this, "BasicShader.vert", "BasicShader.frag");

	ForwardShaderName = m_pShaderManager->CreateShader(this,"BasicShader.vert","BasicShader.frag","BasicShader.tesc","BasicShader.tese", "BasicShader.geom");


	GeoShaderName =	m_pShaderManager->CreateShader(this, "./Shader/Ground_Deferred_GeoPass.vert", "./Shader/Ground_Deferred_GeoPass.frag", "./Shader/Ground_Deferred_GeoPass.tesc",
		"./Shader/Ground_Deferred_GeoPass.tese", "./Shader/Ground_Deferred_GeoPass.geom");
	
	//pDefGeoPass = new MyShader();
	//pDefGeoPass->build_program_from_files("./Shader/Ground_Deferred_GeoPass.vert", "./Shader/Ground_Deferred_GeoPass.frag");
	//pDefLitPass->build_program_from_files("BasicShader.vert", "BasicShader.frag");

	//m_pShaderShadow = new MyShader("./Shader/Shadow_Ground.vert", "./Shader/Shadow_Ground.frag");
	ShadowShaderName = m_pShaderManager->CreateShader(this, "./Shader/Shadow_Ground.vert", "./Shader/Shadow_Ground.frag", 
		"./Shader/Shadow_Ground.tesc", "./Shader/Shadow_Ground.tese", "./Shader/Shadow_Ground.geom");

}

void PatchedGround::Create(GLuint Xcnt, GLuint Zcnt, GLfloat Offset, GLint TileS, GLint TileT)
{
	Vec*			pVertice;
	Vec*			pNormal;
	IndexVector*	pIndices;
	TexCoord*		pTexcoords;
	TexCoord*		pTexcoordForNoiseMap;

	m_MaxTessLevel = 0;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &m_MaxTessLevel);
	m_MaxTessLevel = 1;


	VertexOffset = Offset;
	PatchCountX = Xcnt;
	PatchCountZ = Zcnt;

	VertexCountX = Xcnt+1;
	VertexCountZ = Zcnt+1;
	
	VertexCount = VertexCountX * VertexCountZ;

	m_TileS = TileS;
	m_TileT = TileT;

	//���ؽ� ���� * 3��ŭ �Ҵ�
	pVertice = new Vec[VertexCount];
	pNormal = new Vec[VertexCount];

	pTexcoords = new TexCoord[VertexCount];
	pTexcoordForNoiseMap = new TexCoord[VertexCount];

	//���ʺ��� ����鼭 ������
	float xStart = -Offset*Xcnt/2;
	float zStart = -Offset*Zcnt/2;
	
	//������ �ؽ���
	float* pNoiseData = new float[VertexCount];

	for (int i = 0; i < VertexCount; i++)
	{
		//�븻�� ���߿� ���� ����
		//      nx                             ny                            nz
		pNormal[i].x = 0.f; pNormal[i].y = 1.f; pNormal[i].z = 0.f;
		
		if (i == 65)
		{
			int a = 100;
		}
		//Vertex.x
		pVertice[i].x = xStart + Offset * (i % VertexCountX);
		
		//Vertex.z
		pVertice[i].z = zStart + Offset*(i / VertexCountX);

		
		pVertice[i].y = 0;//GetHeightByNoise(pVertice[i].x, pVertice[i].z);
		
		pNoiseData[i] = pow(GetNoise(pVertice[i].x, pVertice[i].z),3);
	
		pTexcoords[i].x = (float)1 / Xcnt * (i % VertexCountX);
		pTexcoords[i].y = (float)1 / Zcnt * (i / VertexCountX);
	
	}

	TriangleCount = PatchCountX * PatchCountX * 2;
	pIndices = new InVec[TriangleCount];

	int xindex = 0;
	int zindex = 0;
	int j = 0;
	int facecnt = 0;
	
	//�� ���������� ���ؽ��� ���� ���� �ʾƾߵ�
	while (j < VertexCountX * (VertexCountZ - 1))
	{
		if (xindex == PatchCountX)
		{
			xindex = 0;
			zindex++;
			j++;
			continue;
		}
		pIndices[facecnt].A = j;
		pIndices[facecnt].B = j + VertexCountX;
		pIndices[facecnt].C = j + 1;
		facecnt++;

		pIndices[facecnt].A = j + VertexCountX;
		pIndices[facecnt].B = j + VertexCountX + 1;
		pIndices[facecnt].C = j + 1;
		facecnt++;

		xindex++;
		j++;
	}

	MainTexUnit = 11;
	SnowTexUnit = 12;
	StoneTexUnit = 13;
	NoiseTexUnit = 14;
	//�븻 ������ �ڵ� �ʿ�
	NormalReCompute(pVertice, pIndices, pNormal);

	MeshEntry * GroundMesh = new MeshEntry((GLfloat*)pVertice, VertexCount*3, (GLuint*)pIndices, TriangleCount*3, (GLfloat*)pNormal,
									(GLfloat*)pTexcoords, VertexCount*2);
	//GroundMesh->AddTexcoordAttibute((GLfloat*)pTexcoordForNoiseMap , VertexCount * 2);

	Texture* MainTex = new Texture("./Texture/Ground.jpg",MainTexUnit);
	GroundMesh->AddTexture(MainTex);
	Texture* SnowTex = new Texture("./Texture/Snow.jpg", SnowTexUnit);
	GroundMesh->AddTexture(SnowTex);
	Texture* StoneTex = new Texture("./Texture/Stone.jpg", StoneTexUnit);
	GroundMesh->AddTexture(StoneTex);
	Texture* NoiseTex = new Texture();
	NoiseTex->CreateTextureByData(pNoiseData, VertexCountX, VertexCountZ, NoiseTexUnit);
	GroundMesh->AddTexture(NoiseTex);
	
	//GroundMesh->MakeInstancingBuffer();
	
	//�ִ� �� ��ġ��ŭ UBO ������ ��Ƴ��� (�� ����-> float 16�� + int 1��(���� ����) )
	int strSize = sizeof(PaddingLight);
	AddUBO(nullptr, strSize*LIGHT_MAX + sizeof(GLuint),"LightInfoList", 0 , m_pShaderManager->ApplyShaderByName(ForwardShaderName));

	m_pModel = new Model();
	m_pModel->AddMesh(GroundMesh);
	m_pModel->MakeInstancingBuffer();

	m_pModel->SetPrimitiveKind(GL_PATCHES);


	delete[] pVertice;
	delete[] pIndices;
	delete[] pNormal;
	delete[] pTexcoords;
	delete[] pTexcoordForNoiseMap;
	delete[] pNoiseData;
}
void PatchedGround::CreateForTess(GLuint XPatchcnt, GLuint ZPatchcnt, GLfloat PreTessOffset, GLint TileS, GLint TileT)
{
	m_MaxTessLevel = 0;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &m_MaxTessLevel);
	m_MaxTessLevel = VERTICES_OPTIMIZATION_NUM;

	Vec*			pVertice;
	Vec*			pNormal;
	IndexVector*	pIndices;
	TexCoord*		pTexcoords;
	TexCoord*		pTexcoordForNoiseMap;


	// ���ؽ� ����ȭ
	// ������ �پ�縸ŭ ������ �÷��� ũ�� ����
	VertexOffset = PreTessOffset * VERTICES_OPTIMIZATION_NUM;
	PatchCountX = XPatchcnt / VERTICES_OPTIMIZATION_NUM;
	PatchCountZ = ZPatchcnt / VERTICES_OPTIMIZATION_NUM;

	VertexCountX = PatchCountX + 1;
	VertexCountZ = PatchCountZ + 1;

	VertexCount = VertexCountX * VertexCountZ;
	
	// ���ߴ� ��ġ���� 2�踸ŭ ����
	int NoiseTextureX = XPatchcnt* NOISE_SCALE;
	int NoiseTextureZ = ZPatchcnt * NOISE_SCALE;

	float* pNoiseData = new float[NoiseTextureX*NoiseTextureZ];

	float NoiseOffset = PreTessOffset / NOISE_SCALE *SLOPE_CHANGE;
	float NoisexStart = -NoiseOffset*NoiseTextureX / 2;
	float NoisezStart = -NoiseOffset*NoiseTextureZ / 2;

	//������ �ؽ���
	
	for (int i = 0; i < NoiseTextureX*NoiseTextureZ; i++)
	{
		float noiseX = NoisexStart + NoiseOffset * (i % NoiseTextureX);
		float noiseZ = NoisezStart + NoiseOffset*(i / NoiseTextureZ);

		pNoiseData[i] = pow(GetNoise(noiseX, noiseZ), 3);
	}
	
	m_TileS = TileS;
	m_TileT = TileT;

	//���ؽ� ���� * 3��ŭ �Ҵ�
	pVertice = new Vec[VertexCount];
	pNormal = new Vec[VertexCount];

	pTexcoords = new TexCoord[VertexCount];
	pTexcoordForNoiseMap = new TexCoord[VertexCount];

	//���ʺ��� ����鼭 ������
	float xStart = -VertexOffset*PatchCountX / 2;
	float zStart = -VertexOffset*PatchCountZ / 2;

	
	for (int i = 0; i < VertexCount; i++)
	{
		//�븻�� ���߿� ���� ����
		//      nx                             ny                            nz
		pNormal[i].x = 0.f; pNormal[i].y = 1.f; pNormal[i].z = 0.f;

		if (i == 65)
		{
			int a = 100;
		}
		//Vertex.x
		pVertice[i].x = xStart + VertexOffset * (i % VertexCountX);

		//Vertex.z
		pVertice[i].z = zStart + VertexOffset*(i / VertexCountX);

		pVertice[i].y = GetHeightByNoise(pVertice[i].x, pVertice[i].z);


		pTexcoords[i].x = (float)1 / PatchCountX * (i % VertexCountX);
		pTexcoords[i].y = (float)1 / PatchCountZ * (i / VertexCountX);

	}

	TriangleCount = PatchCountX * PatchCountX * 2;
	pIndices = new InVec[TriangleCount];

	int xindex = 0;
	int zindex = 0;
	int j = 0;
	int facecnt = 0;

	//�� ���������� ���ؽ��� ���� ���� �ʾƾߵ�
	while (j < VertexCountX * (VertexCountZ - 1))
	{
		if (xindex == PatchCountX)
		{
			xindex = 0;
			zindex++;
			j++;
			continue;
		}
	
		pIndices[facecnt].A = j;
		pIndices[facecnt].B = j + VertexCountX;
		pIndices[facecnt].C = j + 1;
		facecnt++;

		pIndices[facecnt].A = j + VertexCountX;
		pIndices[facecnt].B = j + VertexCountX + 1;
		pIndices[facecnt].C = j + 1;
		facecnt++;

		xindex++;
		j++;
	}

	MainTexUnit = 11;
	SnowTexUnit = 12;
	StoneTexUnit = 13;
	NoiseTexUnit = 14;
	

	NormalReCompute(pVertice, pIndices, pNormal);

	MeshEntry * GroundMesh = new MeshEntry((GLfloat*)pVertice, VertexCount * 3, (GLuint*)pIndices, TriangleCount * 3, (GLfloat*)pNormal,
		(GLfloat*)pTexcoords, VertexCount * 2);
	//GroundMesh->AddTexcoordAttibute((GLfloat*)pTexcoordForNoiseMap, VertexCount * 2);

	Texture* MainTex = new Texture("./Texture/Ground.jpg", MainTexUnit);
	GroundMesh->AddTexture(MainTex);
	Texture* SnowTex = new Texture("./Texture/Snow.jpg", SnowTexUnit);
	GroundMesh->AddTexture(SnowTex);
	Texture* StoneTex = new Texture("./Texture/Stone.jpg", StoneTexUnit);
	GroundMesh->AddTexture(StoneTex);
	Texture* NoiseTex = new Texture();
	NoiseTex->CreateTextureByData(pNoiseData, NoiseTextureX, NoiseTextureZ, NoiseTexUnit);
	GroundMesh->AddTexture(NoiseTex);

	//GroundMesh->MakeInstancingBuffer();

	//�ִ� �� ��ġ��ŭ UBO ������ ��Ƴ��� (�� ����-> float 16�� + int 1��(���� ����) )
	int strSize = sizeof(PaddingLight);
	AddUBO(nullptr, strSize*LIGHT_MAX + sizeof(GLuint), "LightInfoList", 0, m_pShaderManager->ApplyShaderByName(ForwardShaderName));

	m_pModel = new Model();
	m_pModel->AddMesh(GroundMesh);
	m_pModel->MakeInstancingBuffer();

	m_pModel->SetPrimitiveKind(GL_PATCHES);

	delete[] pVertice;
	delete[] pIndices;
	delete[] pNormal;
	delete[] pTexcoords;
	delete[] pTexcoordForNoiseMap;
	delete[] pNoiseData;


}



double PatchedGround::GetNoise(float x, float z)
{
	//������ǥ�� UV ��ǥ�� ��ȯ
	double perlinx = (double)(x+VertexOffset*PatchCountX / 2) ;
	double perliny = (double)(z + VertexOffset*PatchCountZ / 2) ;
	perlinx = perlinx / (double)(VertexCountX*VertexOffset);
	perliny = perliny / (double)(VertexCountZ*VertexOffset);
	return 1.2*Perlin->noise(4 * perlinx, 4 * perliny, 0.8)
		+ 0.3*Perlin->noise(16 * perlinx, 16 * perliny, 0.8)
		+ 0.05*Perlin->noise(32 * perlinx, 32 * perliny, 0.8);
}
GLfloat PatchedGround::GetXSize()
{
	return VertexOffset*VertexCountX;
	
}
GLfloat PatchedGround::GetZSize()
{
	return VertexOffset*VertexCountZ;
}
double PatchedGround::GetHeightByNoise(float x, float z)
{
	return pow((GetNoise(x*SLOPE_CHANGE,z*SLOPE_CHANGE)), 3) *MaxHeight;
	
}

float* PatchedGround::MakeNoiseTextureData()
{
	float* pData = new float[VertexCount];

	float xStart = -VertexOffset*PatchCountX / 2;
	float zStart = -VertexOffset*PatchCountZ / 2;

	for (int i = 0; i < VertexCount; i++)
	{
	
		double X = xStart + VertexOffset * (i % VertexCountX);
		double Z = zStart + VertexOffset*(i / VertexCountX);

		pData[i]= (float)GetNoise(X, Z);

	}
	return pData;
}
void PatchedGround::NormalReCompute(Vec* pVert, InVec* pind, Vec* pNorm)
{
	for (int FaceCnt = 0; FaceCnt < TriangleCount; FaceCnt++)
	{
		GLuint Aindex = pind[FaceCnt].A;
		GLuint Bindex = pind[FaceCnt].B;
		GLuint Cindex = pind[FaceCnt].C;

		//�� ���̽��� ������
		glm::vec3  APos = glm::vec3(pVert[Aindex].x, pVert[Aindex].y, pVert[Aindex].z);
		glm::vec3  BPos = glm::vec3(pVert[Bindex].x, pVert[Bindex].y, pVert[Bindex].z);
		glm::vec3  CPos = glm::vec3(pVert[Cindex].x, pVert[Cindex].y, pVert[Cindex].z);

		glm::vec3 vBA = APos - BPos;
		glm::vec3 vBC = CPos - BPos;

		glm::vec3 vNorm = glm::cross(vBC, vBA);
		vNorm = glm::normalize(vNorm);
		if (vNorm.y < 0) continue;

		glm::vec3 NewANormal = glm::vec3(pNorm[Aindex].x, pNorm[Aindex].y, pNorm[Aindex].z)
								+ vNorm;
		NewANormal = glm::normalize(NewANormal);
		pNorm[Aindex].x += NewANormal.x; pNorm[Aindex].y += NewANormal.y; pNorm[Aindex].z += NewANormal.z;

		glm::vec3 NewBNormal = glm::vec3(pNorm[Bindex].x, pNorm[Bindex].y, pNorm[Bindex].z)
			+ vNorm;
		NewBNormal = glm::normalize(NewBNormal);
		pNorm[Bindex].x += NewBNormal.x; pNorm[Bindex].y += NewBNormal.y; pNorm[Bindex].z += NewBNormal.z;

		glm::vec3 NewCNormal = glm::vec3(pNorm[Cindex].x, pNorm[Cindex].y, pNorm[Cindex].z)
			+ vNorm;
		NewCNormal = glm::normalize(NewCNormal);
		pNorm[Cindex].x += NewCNormal.x; pNorm[Cindex].y += NewCNormal.y; pNorm[Cindex].z += NewCNormal.z;

	}
}
void PatchedGround::Update(GLfloat dtime)
{
	Object::Update(dtime);
}

GLfloat PatchedGround::GetMaxHeight()
{
	return MaxHeight;
}
void PatchedGround::Render()
{
	

	Object::Render();
}
void PatchedGround::ShaderParamInit(MyShader* ManagedShader)
{

	MyShader* ThisShader = m_pShaderManager->ApplyShaderByName(ForwardShaderName);
	ThisShader->SetUniform1i("TextureGround", MainTexUnit);
	ThisShader->SetUniform1i("TextureSnow", SnowTexUnit);
	ThisShader->SetUniform1i("TextureStone", StoneTexUnit);
	ThisShader->SetUniform1i("SamplerNoise", NoiseTexUnit);

	ThisShader->SetUniform1f("TileS",m_TileS);
	ThisShader->SetUniform1f("TileT" , m_TileT);
	ThisShader->SetUniform1f("gTessLevel", m_MaxTessLevel);
	ThisShader->SetUniform1f("gDispFactor", MaxHeight);
	
	Object::ShaderParamInit(ThisShader);
}