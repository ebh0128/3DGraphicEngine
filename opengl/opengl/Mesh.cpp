#include "CommonHeader.h"
#include "Camera.h"
#include "Texture.h"

#include "Scene.h"
#include "ProgramManager.h"



Model::Model()
{
	m_MainTextureUnitNum = 5;
}

//Assimp용 모델로드 이므로 숨김
void Model::CreateAssimpModel(const aiScene* pAssimpScene, std::string FilePath)
{

	for (int i = 0; i < pAssimpScene->mNumMeshes; i++)
	{
		const aiMesh* AssimpMesh = pAssimpScene->mMeshes[i];

		MeshEntry* NewMesh = new MeshEntry(pAssimpScene, AssimpMesh);
		//텍스쳐 찾기
		aiMaterial* aiMat = pAssimpScene->mMaterials[AssimpMesh->mMaterialIndex];
		aiString TexturePath;
		//텍스쳐의 상대경로를 알아냄 (Object 의 경로로부터의)
		aiReturn ret = aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &TexturePath);
		if (ret == AI_SUCCESS)
		{
			char* TPath = TexturePath.data;
			char* TextureName = strrchr(TPath, '/');
			if (TextureName == nullptr)  TextureName = strrchr(TPath, '\\');
			std::string sDir = FilePath + TextureName;
			Texture* MainTextue = new Texture(sDir.c_str(), m_MainTextureUnitNum);
			NewMesh->AddTexture(MainTextue);
		
		}
		AddMesh(NewMesh);

	}
	MakeInstancingBuffer();
}

//하나의 메쉬덩어리를 모델로 사용할때 이용할것
void Model::CreateMeshEntry(GLfloat* vertices, int VertexNum,
	GLuint* indices, int indicesNum,
	GLfloat* normals,
	GLfloat* texcoords, int texcoordsNum)
{
	MeshEntry* newMesh = new MeshEntry(vertices, VertexNum, indices, indicesNum, normals, texcoords, texcoordsNum);
	m_MeshList.push_back(newMesh);

}

//Assimp에서 모델 로드할때 
void Model::CreateModelFromFile(std::string FilePath, std::string FileName)
{
	Assimp::Importer importer;
	std::string FullPath = FilePath + FileName;
	const aiScene* AssimpScene = importer.ReadFile(FullPath,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals);

	
	CreateAssimpModel( AssimpScene, FilePath);

}
void Model::AddMesh(MeshEntry* pNewMesh)
{
	if (pNewMesh == nullptr) return;
	m_MeshList.push_back(pNewMesh);
}

void Model::Render( GLuint* MatLocation)
{
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		if (MatLocation != nullptr)
		{
			Material* p = m_MeshList[i]->GetMaterial();

			glUniform3fv(MatLocation[0],1, glm::value_ptr(p->diffuse));
			glUniform3fv(MatLocation[1], 1, glm::value_ptr(p->ambient));
			glUniform3fv(MatLocation[2], 1, glm::value_ptr(p->specular));
			glUniform1f(MatLocation[3] , p->shininess);


		}
		m_MeshList[i]->Render();
	}
}

void Model::RenderInstance(int InstanceObjNum , GLuint* MatLocation)
{

	//인스턴스 버퍼 적용
	/*
	for (int i = 0;i < InstanceBufferCount; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, instance_vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16 * InstanceObjNum, InstanceBufferData[i], GL_DYNAMIC_DRAW);

	}
	*/

	for (int i = 0; i < m_MeshList.size(); i++)
	{
		if (MatLocation != nullptr)
		{
			Material* p = m_MeshList[i]->GetMaterial();

			glUniform3fv(MatLocation[0], 1, glm::value_ptr(p->diffuse));
			glUniform3fv(MatLocation[1], 1, glm::value_ptr(p->ambient));
			glUniform3fv(MatLocation[2], 1, glm::value_ptr(p->specular));
			glUniform1f(MatLocation[3], p->shininess);

		}
		m_MeshList[i]->RenderInstance(InstanceObjNum);
	}
}



void Model::MakeInstancingBuffer(int iternum)
{
	/*
	glBindVertexArray(vao);

	for (int i = 0; i < iternum; i++)
	{

		glGenBuffers(1, &instance_vbo[InstanceBufferCount]);
		glBindBuffer(GL_ARRAY_BUFFER, instance_vbo[InstanceBufferCount]);
		for (int i = 0;i < 4; i++)
		{
			glEnableVertexAttribArray(attrib_MVPMat + i + 4 * InstanceBufferCount);
			glVertexAttribPointer(attrib_MVPMat + i + 4 * InstanceBufferCount, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 16, (const GLvoid*)(sizeof(GLfloat)*i * 4));
			glVertexAttribDivisor(attrib_MVPMat + i + 4 * InstanceBufferCount, 1);
		}
		InstanceBufferCount++;
	}
	*/
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		m_MeshList[i]->MakeInstancingBuffer(iternum);
	}
}

void Model::SetInstanceBufferData(glm::mat4 *pData, int Index)
{
	//if (pData == nullptr) return;

	//InstanceBufferData[Index] = pData;
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		m_MeshList[i]->SetInstanceBufferData(pData, Index);
	}
}











MeshEntry::MeshEntry()
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
MeshEntry::MeshEntry(GLfloat* vertices, int VertexNum,
	GLuint* indices, int IndicesNum,
	GLfloat* normals,
	GLfloat* texcoords, int texcoordsNum) : MeshEntry()
{

	ObjectLoad(vertices, VertexNum,indices, IndicesNum, normals, texcoords, texcoordsNum);
}

MeshEntry::MeshEntry(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh) : MeshEntry()
{

	ObjectLoad(pAssimpScene, pAssimpMesh);

}

MeshEntry::~MeshEntry()
{


}
GLfloat* MeshEntry::NormalGenerate(GLfloat* vertices, GLuint* indices)
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

void MeshEntry::ObjectLoad(const aiScene* pAssimpScene, const aiMesh* pAssimpMesh)
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
	//MakeInstancingBuffer();
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void MeshEntry::ObjectLoad(GLfloat* vertices, int VertexNum,
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

void MeshEntry::AddTexcoordAttibute(void* texcoords, int TexcoordNum)
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





void MeshEntry::AddTexture(Texture* pNewTex)
{
	if (pNewTex == nullptr) return;
	pTextureList.push_back(pNewTex);
}

void MeshEntry::Render()
{
	
	glBindVertexArray(vao);
	glEnableVertexAttribArray(attrib_Pos);
	if(IsHaveNormal) glEnableVertexAttribArray(attrib_Normal);
	
	//샘플러가 있다면 적용
	
	
	//하드코딩 일단 2개쓰므로
	glEnableVertexAttribArray(attrib_Texcoord );
	glEnableVertexAttribArray(attrib_Texcoord+1);

	for (int i = 0; i < pTextureList.size(); i++)
	{
		pTextureList[i]->ApplyTexture();
	}


	//if (ubo > 0) glBindBuffer(GL_UNIFORM_BUFFER, ubo);

	glDrawElements(GL_TRIANGLES, PrimCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}
void MeshEntry::RenderInstance(int InstanceObjCount)
{
	
	for (int i = 0;i < InstanceBufferCount; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, instance_vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16 * InstanceObjCount, InstanceBufferData[i], GL_DYNAMIC_DRAW);

	}
	
	
	
	glBindVertexArray(vao);
	glEnableVertexAttribArray(attrib_Pos);
	if (IsHaveNormal) glEnableVertexAttribArray(attrib_Normal);

	//샘플러가 있다면 적용
	for (int i = 0; i < TexCoordCnt; i++)
	{
		if (IsHaveTexcoord)glEnableVertexAttribArray(attrib_Texcoord + i);
		//pMeshSamplerList[i]->ApplySampler(IsDeferred);

	}
	
	for (int i = 0; i < pTextureList.size(); i++)
	{
		pTextureList[i]->ApplyTexture();
	}
	
	GLuint StartIndex= 0;
	//if (ubo > 0) glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
										PrimCount,
										GL_UNSIGNED_INT,
										(void*) (StartIndex),
										InstanceObjCount,
										0
										);
	
}


//어떻게 넘겨줘도 대응해야되므로 깊은복사
void MeshEntry::SetMaterial(Material* pmat)
{
	material->ambient = pmat->ambient;
	material->diffuse = pmat->diffuse;
	material->shininess = pmat->shininess;
	material->specular = pmat->specular;
}
Material* MeshEntry::GetMaterial()
{
	return material;
}

void MeshEntry::MakeInstancingBuffer(int iternum)
{
		glBindVertexArray(vao);

	for (int i = 0; i < iternum; i++)
	{

		glGenBuffers(1, &instance_vbo[InstanceBufferCount]);
		glBindBuffer(GL_ARRAY_BUFFER, instance_vbo[InstanceBufferCount]);
		for (int i = 0;i < 4; i++)
		{
			glEnableVertexAttribArray(attrib_MVPMat + i + 4 * InstanceBufferCount);
			glVertexAttribPointer(attrib_MVPMat + i + 4 * InstanceBufferCount, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 16, (const GLvoid*)(sizeof(GLfloat)*i * 4));
			glVertexAttribDivisor(attrib_MVPMat + i + 4 * InstanceBufferCount, 1);
		}
		InstanceBufferCount++;
	}
}

void MeshEntry::SetInstanceBufferData(glm::mat4 *pData, int Index)
{
	if (pData == nullptr) return;

	InstanceBufferData[Index] = pData;
}
