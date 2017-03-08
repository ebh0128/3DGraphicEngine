#include "CommonHeader.h"
#include "MyShader.h"
#include "Camera.h"
#include "Texture.h"
#include "Sampler.h"
#include "Object.h"
#include "Mesh.h"

#include "Scene.h"
#include "ProgramManager.h"





Mesh::Mesh()
{
	material = new Material();
	material->diffuse = glm::vec4(1.f, 1.f, 1.f, 1.f);
	material->ambient = glm::vec4(.3f, .3f, .3f, .3f);
	material->specular = glm::vec4(0.f, 0.f, 0.f, 0.f);
	material->shininess = 1.f;
	TexCoordCnt = 0;
	VerticesCount = 0;


	IsHaveNormal =false;
	IsHaveTexcoord = false;


}
Mesh::Mesh(GLfloat* vertices, int VertexNum,
	GLuint* indices, int IndicesNum,
	GLfloat* normals,
	GLfloat* texcoords, int texcoordsNum) : Mesh()
{

	ObjectLoad(vertices, VertexNum,indices, IndicesNum, normals, texcoords, texcoordsNum);
}

Mesh::Mesh(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh) : Mesh()
{

	ObjectLoad(pAssimpScene, pAssimpMesh);

}

Mesh::~Mesh()
{


}
GLfloat* Mesh::NormalGenerate(GLfloat* vertices, GLuint* indices)
{
#define x(i) vertices[3*i]
#define y(i) vertices[3*i+1]
#define z(i) vertices[3*i+2]

	//버텍스만큼 노말 필요
	GLfloat* Normals = new GLfloat[_msize(vertices)];

	
	for (int i = 0; i < _msize(indices) ; i +=3)
	{
		
		glm::vec3 p0 = glm::vec3(x(indices[i]), y(indices[i]), z(indices[i]));
		glm::vec3 p1 = glm::vec3(x(indices[i+1]), y(indices[i + 1]), z(indices[i + 1]));
		glm::vec3 p2 = glm::vec3(x(indices[i+2]), y(indices[i + 2]), z(indices[i + 2]));

		glm::vec3 norm = glm::normalize( glm::cross((p2 - p1), (p0 - p1)));
		//n0
		Normals[3 * i] = norm.x; Normals[3 * i+1] = norm.y; Normals[3 * i+2] = norm.z;
		//n1
		Normals[3 * (i+1)] = norm.x; Normals[3 * (i + 1) + 1] = norm.y; Normals[3 * (i + 1) + 2] = norm.z;
		//n2
		Normals[3 * (i + 2)] = norm.x; Normals[3 * (i + 2) + 1] = norm.y; Normals[3 * (i + 2) + 2] = norm.z;

	}

	return Normals;
#undef x
#undef y
#undef z
}

void Mesh::ObjectLoad(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh)
{
	if (pAssimpScene == nullptr || pAssimpMesh == nullptr)
	{
		printf("잘못된 Assimp Mesh Load\n");
		return;
	}
	//씬에서 가지고있는 메테리얼 정보를 인덱스를 통해 가져옴
	aiMaterial *pMaterial = pAssimpScene->mMaterials[pAssimpMesh->mMaterialIndex];
	aiColor4D aiDiffuse , aiSpec, aiAmbi ;
	
	aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &aiDiffuse);
	aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &aiSpec);
	aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &aiAmbi);
	
	//aiColor -> 앱에서 사용하는 메테리얼로 전환
	material->diffuse.r = aiDiffuse.r; material->diffuse.g = aiDiffuse.g; material->diffuse.b = aiDiffuse.b; material->diffuse.a = aiDiffuse.a;
	material->specular.r = aiSpec.r; material->specular.g = aiSpec.g; material->specular.b = aiSpec.b; material->specular.a = aiSpec.a;
	material->ambient.r = aiAmbi.r; material->ambient.g = aiAmbi.g; material->ambient.b = aiAmbi.b; material->ambient.a = aiAmbi.a;

	unsigned int max;
	aiGetMaterialFloatArray(pMaterial, AI_MATKEY_SHININESS, &(material->shininess),&max);


	
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	//포지션
	VerticesCount = pAssimpMesh->mNumVertices;
	if (pAssimpMesh->HasPositions())
	{
		
		glGenBuffers(1, &vbo_position);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*VerticesCount*3, (GLfloat*)&pAssimpMesh->mVertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(attrib_Pos);
		glVertexAttribPointer(attrib_Pos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	}
	if (pAssimpMesh->HasNormals())
	{
		IsHaveNormal = true;
		glGenBuffers(1, &vbo_normal);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*VerticesCount*3, (GLfloat*)&pAssimpMesh->mNormals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(attrib_Normal);
		glVertexAttribPointer(attrib_Normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	}
	
	if (pAssimpMesh->HasFaces())
	{
		
		std::vector<GLuint> Tempindices;
		// i  = facenumber
		for (int i = 0; i < pAssimpMesh->mNumFaces; i++)
		{
			//j = index number
			for (int j = 0; j < pAssimpMesh->mFaces[i].mNumIndices; j++)
			{
				Tempindices.push_back(pAssimpMesh->mFaces[i].mIndices[j]);
			}
		}
		
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*Tempindices.size(), Tempindices.data(), GL_STATIC_DRAW);
		PrimCount = Tempindices.size();
	}
	
	//텍스쳐 좌표는 여러개 존재 가능
	int TexCnt = 0;
	float *temptex = new float[VerticesCount * 2];

	while(pAssimpMesh->HasTextureCoords(TexCnt))
	{
		memset(temptex, 0, sizeof(float)*VerticesCount * 2);
		for (int i = 0; i < VerticesCount; i++)
		{
			temptex[2 * i] = pAssimpMesh->mTextureCoords[TexCnt][i].x;
			temptex[2 * i+1] = pAssimpMesh->mTextureCoords[TexCnt][i].y;
		}

		//AddTexcoordAttibute(&pAssimpMesh->mTextureCoords[TexCnt][0], VerticesCount * 2);
		AddTexcoordAttibute(temptex, VerticesCount * 2);
		TexCnt++;
	}
	MakeInstancingBuffer();
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Mesh::ObjectLoad(GLfloat* vertices, int VertexNum,
					GLuint* indices, int IndicesNum,
					GLfloat* normals,
					GLfloat* texcoords, int texcoordsNum)
{

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_position);
	//포지션
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*VertexNum, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(attrib_Pos);
	glVertexAttribPointer(attrib_Pos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	
	//노말
	if (normals != nullptr)
	{
		IsHaveNormal = true;
		glGenBuffers(1, &vbo_normal);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*VertexNum, normals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(attrib_Normal);
		glVertexAttribPointer(attrib_Normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	}
	
	if (texcoords != nullptr)
	{
		AddTexcoordAttibute(texcoords, texcoordsNum);
		
	}
	
	//인덱스
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*IndicesNum, indices, GL_STATIC_DRAW);

	PrimCount = IndicesNum;
	//초기화



	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::AddTexcoordAttibute(void* texcoords, int TexcoordNum)
{
	if (texcoords == nullptr) return;
	glBindVertexArray(vao);
	IsHaveTexcoord = true;

	glGenBuffers(1, &(vbo_texcoord[TexCoordCnt]));
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord[TexCoordCnt]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*TexcoordNum, texcoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attrib_Texcoord + TexCoordCnt);
	glVertexAttribPointer(attrib_Texcoord + TexCoordCnt, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	TexCoordCnt++;
	
	//초기화는 하지 말것 (대신 사용시 주의)
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void Mesh::MakeInstancingBuffer()
{
	glBindVertexArray(vao);

	glGenBuffers(1, &instance_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
	for (int i = 0;i < 4; i++)
	{
		glEnableVertexAttribArray(attrib_MVPMat + i);
		glVertexAttribPointer(attrib_MVPMat + i, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 16, (const GLvoid*)(sizeof(GLfloat)*i * 4));
		glVertexAttribDivisor(attrib_MVPMat + i, 1);
	}
}

Sampler*  Mesh::CreateSampler(const char* ImagePath, GLuint Channel, GLint ShaderLoc, GLint DefShaderLoc)
{
	Sampler* pNewSampler = new Sampler(ImagePath, Channel, ShaderLoc, DefShaderLoc);
	AddSampler(pNewSampler);
	return pNewSampler;
	
}



void Mesh::AddSampler(Sampler* pNewSamp)
{
	if (pNewSamp == nullptr) return;
	pMeshSamplerList.push_back(pNewSamp);
}

void Mesh::Render(int IsDeferred)
{
	
	glBindVertexArray(vao);
	glEnableVertexAttribArray(attrib_Pos);
	if(IsHaveNormal) glEnableVertexAttribArray(attrib_Normal);
	
	//샘플러가 있다면 적용
	for (int i = 0; i < pMeshSamplerList.size(); i++)
	{
		if (IsHaveTexcoord)glEnableVertexAttribArray(attrib_Texcoord+i);
		pMeshSamplerList[i]->ApplySampler(IsDeferred);

	}
	
	//if (ubo > 0) glBindBuffer(GL_UNIFORM_BUFFER, ubo);

	glDrawElements(GL_TRIANGLES, PrimCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}
void Mesh::Render(glm::mat4 * MVPmats, unsigned int InstanceNum , int IsDeferred)
{

	
	glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16 * InstanceNum, MVPmats, GL_DYNAMIC_DRAW);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(attrib_Pos);
	if (IsHaveNormal) glEnableVertexAttribArray(attrib_Normal);

	//샘플러가 있다면 적용
	for (int i = 0; i < pMeshSamplerList.size(); i++)
	{
		if (IsHaveTexcoord)glEnableVertexAttribArray(attrib_Texcoord + i);
		pMeshSamplerList[i]->ApplySampler(IsDeferred);

	}
	GLuint StartIndex= 0;
	//if (ubo > 0) glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
										PrimCount,
										GL_UNSIGNED_INT,
										(void*) (StartIndex),
										InstanceNum,
										0
										);
	//glDrawElements(GL_TRIANGLES, PrimCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}
//어떻게 넘겨줘도 대응해야되므로 깊은복사
void Mesh::SetMaterial(Material* pmat)
{
	material->ambient = pmat->ambient;
	material->diffuse = pmat->diffuse;
	material->shininess = pmat->shininess;
	material->specular = pmat->specular;
}
Material* Mesh::GetMaterial()
{
	return material;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//Node///////////////////////////////////////////////////////////////////////////////////

Node::Node()
{


	vPos = glm::vec4(0.f, 0.f, 0.f, 1.f);
	vRot = glm::vec3(0.f, 0.f, 0.f);
	vScale = glm::vec3(1.f, 1.f, 1.f);
	TransformMat = glm::mat4();

	ubo = 0;
	UbSize = 0;

}
Node::Node(Node* _parent, SceneGL* scene) : Node()
{
	Parent = _parent;
	if (Parent != nullptr) Parent->AddChild(this);
	pScene = scene;
}
Node::~Node()
{
	/*
	if (Parent)  Parent = NULL;
	for (GLuint i = 0; i<Children.size(); i++)
	{
		delete Children[i];
	}
	Children.empty();
	for (GLuint i = 0; i<meshes.size(); i++)
	{
		delete meshes[i];
	}
	meshes.empty();
	*/
}
void Node::Update(GLfloat dtime)
{
	//자기자신 Update(즉 모델공간)
	glm::mat4  mTrans = glm::translate(glm::vec3(vPos));

	//회전의 경우 짐벌락등 여러 문제 발생가능
	//나중에 쿼터니언이나 Local 축 기준 회전 이용 고려
	glm::mat4  mRotate = glm::rotate(glm::mat4(), vRot.x, glm::vec3(1.f, 0.f, 0.f));
	mRotate = glm::rotate(mRotate, vRot.y, glm::vec3(0.f, 1.f, 0.f));
	mRotate = glm::rotate(mRotate, vRot.z, glm::vec3(0.f, 0.f, 1.f));

	glm::mat4  mScale = glm::scale(vScale);

	//S > R > T OpenGL에서는 뒤에서부터 읽음
	TransformMat = mTrans * mRotate * mScale;


	//자식 Update(Mesh는 관련 없음)
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->Update(dtime);
	}
}




void Node::AddUBO(void* Data, GLuint Size, const char* BlockName, GLuint* Offset, MyShader* pshad)
{
	GLuint BlockIndex, BindingPoint = 1;
	GLuint ShaderID = pshad->GetShaderProgram();
	BlockIndex = glGetUniformBlockIndex(ShaderID, BlockName);
	glUniformBlockBinding(ShaderID, BlockIndex, BindingPoint);

	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);

	glGetActiveUniformBlockiv(ShaderID, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &UbSize);


	//Uniform Block 쿼리 
	GLchar* name[6] = { "Count" , "List[0].LPos" , "List[0].LDiff" ,
		"List[0].LAmbi" , "List[0].LSpec" , "List[1].LPos" };
	GLuint Index[6];
	GLint offset[6];
	GLint size[6];
	GLint type[6];

	glGetUniformIndices(ShaderID, 6, name, Index);
	glGetActiveUniformsiv(ShaderID, 6, Index, GL_UNIFORM_OFFSET, offset);
	glGetActiveUniformsiv(ShaderID, 6, Index, GL_UNIFORM_SIZE, size);
	glGetActiveUniformsiv(ShaderID, 6, Index, GL_UNIFORM_TYPE, type);


	glBufferData(GL_UNIFORM_BUFFER, UbSize, Data, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, BindingPoint, ubo);




}
void Node::UpdateUBO(void* Data, GLuint Size, GLuint Offset)
{
	if (ubo <= 0) return;

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, Offset, Size, Data);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);
	//LightList* d = (LightList*)Data;
}
void Node::AddMesh(Mesh* pmesh)
{
	meshes.push_back(pmesh);
	int a = meshes.size();
}
void Node::AddChild(Node* pNode)
{
	Children.push_back(pNode);
}

glm::mat4  Node::GetModelMat()
{
	
	return (Parent == nullptr)? TransformMat: TransformMat*Parent->GetModelMat();
}
void Node::SetMeshMaterial(Material* mat, int Index)
{
	if (Index < meshes.size())
		meshes[Index]->SetMaterial(mat);
}
void Node::SetPosition(float x, float y, float z)
{
	vPos.x = x; vPos.y = y; vPos.z = z;
}
void Node::SetObject(Object* Obj)
{
	pObj = Obj;
	for (int i = 0; i < Children.size(); i++)
	{
		Children[i]->SetObject(Obj);
	}
}
void Node::Render()
{

	if (pShader) pShader->ApplyShader();

	pScene->ApplySpotLight(pShader);
	for (GLuint i = 0; i<meshes.size(); i++)
	{

		//메쉬별 쉐이더 재질 적용 
		Material* p = meshes[i]->GetMaterial();

		pShader->SetUniform3fv("material.diffuse", glm::value_ptr(p->diffuse));
		pShader->SetUniform3fv("material.amdient", glm::value_ptr(p->ambient));
		pShader->SetUniform3fv("material.specular", glm::value_ptr(p->specular));
		pShader->SetUniform1f("material.shininess", p->shininess);

		ShaderParamInit();
		//GeoPassInit();
		// 변환 행렬 쉐이더 전송
		//meshes[i]->Render();
		if (pObj == nullptr || pObj->GetInstanceNum() == 0) meshes[i]->Render();
		else meshes[i]->Render(pObj->GetInstanceMatrixData(), pObj->GetInstanceNum());

	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->Render();
	}


}
void Node::ShaderParamInit()
{
	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 M;

	if (Parent == nullptr) M = TransformMat;
	else  M = TransformMat*Parent->GetModelMat();

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;
	pShader->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
	pShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	pShader->SetUniformMatrix4fv("V", glm::value_ptr(V));
	pShader->SetUniformMatrix4fv("World", glm::value_ptr(M));
	// 빛 정보 UiformBlock 쉐이더 전송 
	LightList* DataforShader = pScene->GetLightSrouceArray();
	GLuint Size = DataforShader->Count * sizeof(GLfloat) * 16;
	//meshes[i]->UpdateUBO(DataforShader, Size+ sizeof(GLuint), 0);
	UpdateUBO(DataforShader, sizeof(GLuint), 0);

	// std140 stride 16
	UpdateUBO(DataforShader, Size, 12);

}
void Node::GeoPassInit()
{
	// 변환 행렬 쉐이더 전송

	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 M;

	if (Parent == nullptr) M = TransformMat;
	else  M = TransformMat*Parent->GetModelMat();

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;

	pDefGeoPass->SetUniformMatrix4fv("WVP", glm::value_ptr(MVP));
	pDefGeoPass->SetUniformMatrix4fv("World", glm::value_ptr(M));
	

}


void Node::RenderGeoPass()
{
	if (!pDefGeoPass)  return;
	pDefGeoPass->ApplyShader();

	for (GLuint i = 0; i<meshes.size(); i++)
	{

		// 변환 행렬 쉐이더 전송
		GeoPassInit();
		if (pObj->GetInstanceNum() == 0) meshes[i]->Render(1);
		else meshes[i]->Render(pObj->GetInstanceMatrixData(), pObj->GetInstanceNum(),1);

	}
	for (GLuint i = 0; i<Children.size(); i++)
	{
		Children[i]->RenderGeoPass();
		//Children[i]->Render();
	}

}
