#pragma once

class Water : public Object
{
public:
	Water();
	//메쉬 생성은 Create 호출
	Water(Object* Parent, SceneGL *Scene, GLfloat MaxH);

	void Create(GLuint Xcnt, GLuint Zcnt, GLfloat Offset, GLint TileS, GLint TileT);
	void Render();
	void Update(GLfloat dtime);
	void ShaderParamInit(MyShader* ManagedShader);

protected:
	int m_VertexCount;
	

	int m_VertexCountX;
	int m_VertexCountZ;

	int m_TriangleCount;

	int m_PatchCountX;
	int m_PatchCountZ;

	int m_TileS;
	int m_TileT;

	GLfloat m_DeltaTexCoordS;
	GLfloat m_DeltaTexCoordT;
	GLfloat m_RotAngle;
	GLfloat m_VertexOffset;
	GLfloat m_MaxHeight;

	GLuint m_DiffuseTexUnit;
	GLuint m_NormalTexUnit;
	GLuint m_DisplaceTexUnit;
};