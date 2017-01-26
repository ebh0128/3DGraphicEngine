#pragma once
#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Scene.h"
#include "PerlinNoise.h"
#include "PatchedGround.h"


PatchedGround::PatchedGround()
{

}
PatchedGround::PatchedGround(Node* Parent, SceneGL *Scene, int seed, GLfloat MaxH) :Node(Parent, Scene)
{
	VertexCount = 0;
	TriangleCount = 0;
	NoiseSeed = seed;
	Perlin = new PerlinNoise(seed);
	MaxHeight = MaxH;

	pShader = new MyShader();
	pShader->build_program_from_files("BasicShader.vert", "BasicShader.frag");

}

void PatchedGround::Create(GLuint Xcnt, GLuint Zcnt, GLfloat Offset, GLint TileS, GLint TileT)
{
	Vec*			pVertice;
	Vec*			pNormal;
	IndexVector*	pIndices;
	TexCoord*		pTexcoords;
	TexCoord*		pTexcoordForNoiseMap;


	VertexOffset = Offset;
	PatchCountX = Xcnt;
	PatchCountZ = Zcnt;

	VertexCountX = Xcnt+1;
	VertexCountZ = Zcnt+1;
	
	VertexCount = VertexCountX * VertexCountZ;

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

		
		pVertice[i].y = GetHeightByNoise(pVertice[i].x, pVertice[i].z);
		
		pNoiseData[i] = pow(GetNoise(pVertice[i].x, pVertice[i].z),3);
		//printf
		//Texcoord.s
		pTexcoords[i].x = (float)TileS / Xcnt * (i % VertexCountX);
		pTexcoordForNoiseMap[i].x = (float)1 / Xcnt * (i % VertexCountX);
		//Texcoord.t
		pTexcoords[i].y = (float)TileT / Zcnt * (i / VertexCountX);
		pTexcoordForNoiseMap[i].y = (float)1 / Zcnt * (i / VertexCountX);

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
		//z�� ¦�� �����϶�
		if (zindex % 2 == 0)
		{
			if (xindex % 2 == 1)
			{
				pIndices[facecnt].A = j;
				pIndices[facecnt].B = j + VertexCountX;
				pIndices[facecnt].C = j + VertexCountX +1;
				facecnt++;

				pIndices[facecnt].A = j+1;
				pIndices[facecnt].B = j;
				pIndices[facecnt].C = j +VertexCountX + 1;
				facecnt++;

			}
			else
			{
				pIndices[facecnt].A = j;
				pIndices[facecnt].B = j + VertexCountX;
				pIndices[facecnt].C = j + 1;
				facecnt++;

				pIndices[facecnt].A = j + VertexCountX;
				pIndices[facecnt].B = j + VertexCountX +1;
				pIndices[facecnt].C = j + 1;
				facecnt++;
			}
		}
		//z�� Ȧ�� �����϶�
		else
		{ 
			if (xindex % 2 == 1)
			{
				pIndices[facecnt].A = j;
				pIndices[facecnt].B = j + VertexCountX;
				pIndices[facecnt].C = j + 1;
				facecnt++;

				pIndices[facecnt].A = j + 1;
				pIndices[facecnt].B = j + VertexCountX;
				pIndices[facecnt].C = j + VertexCountX + 1;
				facecnt++;

			}
			else
			{
				pIndices[facecnt].A = j;
				pIndices[facecnt].B = (j + VertexCountX);
				pIndices[facecnt].C = (j + VertexCountX + 1);
				facecnt++;

				pIndices[facecnt].A = j;
				pIndices[facecnt].B = (j + VertexCountX + 1);
				pIndices[facecnt].C = (j + 1);
				facecnt++;

			}
		}

		xindex++;
		j++;
	}

	//�븻 ������ �ڵ� �ʿ�
	NormalReCompute(pVertice, pIndices, pNormal);

	Mesh * GroundMesh = new Mesh((GLfloat*)pVertice, VertexCount*3, (GLuint*)pIndices, TriangleCount*3, (GLfloat*)pNormal,
									(GLfloat*)pTexcoords, VertexCount*2);
	GroundMesh->AddTexcoordAttibute((GLfloat*)pTexcoordForNoiseMap , VertexCount * 2);
	
	Sampler* MainSampler = GroundMesh->CreateSampler("./Texture/Ground.jpg", ShaderManager::GetInstance()->GetChannelID(), glGetUniformLocation(pShader->GetShaderProgram(), "TextureGround"));
	
	MainSampler->CreateTexture("./Texture/Snow.jpg", ShaderManager::GetInstance()->GetChannelID(), glGetUniformLocation(pShader->GetShaderProgram(), "TextureSnow"));
	MainSampler->CreateTexture("./Texture/Stone.jpg", ShaderManager::GetInstance()->GetChannelID(), glGetUniformLocation(pShader->GetShaderProgram(), "TextureStone"));
	//Noise �ؽ��� �����
	Sampler* NoiseSampler = new Sampler();
	NoiseSampler->CreateTextureByData(pNoiseData, VertexCountX, VertexCountZ, ShaderManager::GetInstance()->GetChannelID(), glGetUniformLocation(pShader->GetShaderProgram(), "SamplerNoise"));
	
	GroundMesh->AddSampler(NoiseSampler);
	AddMesh(GroundMesh);
	
	//�ִ� �� ��ġ��ŭ UBO ������ ��Ƴ��� (�� ����-> float 16�� + int 1��(���� ����) )
	AddUBO(nullptr,16 * sizeof(GLfloat)*LIGHT_MAX + sizeof(GLuint),"LightInfoList", 0);

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
	return pow((GetNoise(x,z)), 3) *MaxHeight;
	
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
	Node::Update(dtime);
}

GLfloat PatchedGround::GetMaxHeight()
{
	return MaxHeight;
}
void PatchedGround::Render()
{
	Node::Render();
	/*
	if (pShader) pShader->ApplyShader();

	
	//���� ���� �����̱⿡ World Ʈ������ ����

	for (GLuint i = 0; i<meshes.size(); i++)
	{

		//�޽��� ���̴� ���� ���� 
		Material* p = meshes[i]->GetMaterial();

		pShader->SetUniform3fv("material.diffuse", glm::value_ptr(p->diffuse));
		pShader->SetUniform3fv("material.amdient", glm::value_ptr(p->ambient));
		pShader->SetUniform3fv("material.specular", glm::value_ptr(p->specular));
		pShader->SetUniform1f("material.shininess", p->shininess);

		// ��ȯ ��� ���̴� ����
		glm::mat4 V = pScene->GetVMatrix();
		glm::mat4 VP = pScene->GetVPMatrix();
		glm::mat4 MV = V*TransformMat;
		glm::mat4 MVP = VP*TransformMat;
		pShader->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
		pShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
		pShader->SetUniformMatrix4fv("V", glm::value_ptr(V));
		// �� ���� UiformBlock ���̴� ���� 
		LightList* DataforShader = pScene->GetLightSrouceArray();
		GLuint Size = DataforShader->Count * sizeof(GLfloat) * 16;
		UpdateUBO(DataforShader, sizeof(GLuint), 0);
		// std140 stride 16
		UpdateUBO(DataforShader, Size, 12);
		meshes[i]->Render();
	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->Render();
	}
	*/
}