#pragma once

//여러개의 Triangle로 구성된 Patched Mesh
// 마름모 모양으로 구성되게 index를 꼬아서 만듬
class Object;
class PerlinNoise;




// 해당 상수만큼 버텍스 나눠서 생성
// 테셀을 이용해서 그럼
// 변화가 격렬할수록 더 많은 샘플이 필요함
#define VERTICES_OPTIMIZATION_NUM 16 
#define NOISE_SCALE 2
//경사도 격렬한 정도
//노이즈의 진동수
#define SLOPE_CHANGE 2

class PatchedGround : public Object
{
private:
	PerlinNoise* Perlin;
	int NoiseSeed;
	//진폭(Perlin noise의 결과값이 0~1사이 이므로)
	GLfloat MaxHeight;
	
	// 버텍스 개수
	int VertexCount;

	int VertexCountX;
	int VertexCountZ;
	// 삼각형 개수
	int TriangleCount;

	//패치 개수
	int PatchCountX;
	int PatchCountZ;

	int m_TileS;
	int m_TileT;

	GLfloat VertexOffset;
	typedef struct Vector
	{
		GLfloat x; GLfloat y; GLfloat z;
	}Vec;
	typedef struct IndexVector
	{
		GLuint A; GLuint B; GLuint C;
	}InVec;

	typedef struct TexCoord
	{
		GLfloat x; GLfloat y; 
	}Tex;
	GLuint MainTexUnit;
	GLuint SnowTexUnit;
	GLuint StoneTexUnit;
	GLuint NoiseTexUnit;

	GLint m_MaxTessLevel;
private:
	//버텍스가 Noise나 Height Map에 의해 변경되면 재계산이 필요하다.

	void NormalReCompute(Vec* pVert, InVec* pind, Vec* pNorm);
public:
	PatchedGround();
	
	//메쉬 생성은 Create 호출
	PatchedGround(Object* Parent, SceneGL *Scene, int seed, GLfloat MaxH);

	//메쉬 생성 함수
	void Create(GLuint Xcnt, GLuint Zcnt, GLfloat Offset, GLint TileS, GLint TileT);
	void CreateForTess(GLuint Xcnt, GLuint Zcnt, GLfloat Offset, GLint TileS, GLint TileT);

	double GetNoise(float x, float z);
	double GetHeightByNoise(float x, float z);
	//사용안해도 될듯 (추후 삭제가능)
	float* MakeNoiseTextureData();
	void Update(GLfloat dtime);
	GLfloat GetMaxHeight();
	GLfloat GetXSize();
	GLfloat GetZSize();

	void Render();
	virtual void ShaderParamInit(MyShader* ManagedShader = nullptr);


	
	GLuint GetMainTexUnit() { return MainTexUnit; }
	GLuint GetSnowTexUnit() { return SnowTexUnit; }
	GLuint GetStoneTexUnit() { return StoneTexUnit; }
	GLuint GetNoiseTexUnit() { return NoiseTexUnit; }

	GLuint GetTileS() { return m_TileS; }
	GLuint GetTileT() { return m_TileT; }
	GLint GetTessLevel() { return m_MaxTessLevel; }
};