#pragma once

//�������� Triangle�� ������ Patched Mesh
// ������ ������� �����ǰ� index�� ���Ƽ� ����

class PerlinNoise;
class PatchedGround : public Node
{
private:
	PerlinNoise* Perlin;
	int NoiseSeed;
	//����(Perlin noise�� ������� 0~1���� �̹Ƿ�)
	GLfloat MaxHeight;
	
	// ���ؽ� ����
	int VertexCount;

	int VertexCountX;
	int VertexCountZ;
	// �ﰢ�� ����
	int TriangleCount;

	//��ġ ����
	int PatchCountX;
	int PatchCountZ;

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
private:
	//���ؽ��� Noise�� Height Map�� ���� ����Ǹ� ������ �ʿ��ϴ�.

	void NormalReCompute(Vec* pVert, InVec* pind, Vec* pNorm);
public:
	PatchedGround();
	
	//�޽� ������ Create ȣ��
	PatchedGround(Node* Parent, SceneGL *Scene, int seed, GLfloat MaxH);

	//�޽� ���� �Լ�
	void Create(GLuint Xcnt, GLuint Zcnt, GLfloat Offset, GLint TileS, GLint TileT);
	
	double GetNoise(float x, float z);
	double GetHeightByNoise(float x, float z);
	//�����ص� �ɵ� (���� ��������)
	float* MakeNoiseTextureData();
	void Update(GLfloat dtime);
	GLfloat GetMaxHeight();
	GLfloat GetXSize();
	GLfloat GetZSize();
	void Render();
};