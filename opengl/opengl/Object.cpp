#include "CommonHeader.h"
#include "Camera.h"

#include "Object.h"
#include "Scene.h"


#include "DirLight.h"
Object::Object()
{
	vPos = glm::vec4(0.f, 0.f, 0.f, 1.f);
	vRot = glm::vec3(0.f, 0.f, 0.f);
	vScale = glm::vec3(1.f, 1.f, 1.f);
	TransformMat = glm::mat4();

	ubo = 0;
	UbSize = 0;
	MainTextureUnit = 0;
	m_pModel = nullptr;
	m_bUpdated = false;
}

Object::Object( Object* Parent, SceneGL* Sce) :Object()
{
	//pRoot = Root;
	//pRoot->SetObject(this);
	mParent = Parent;
	if(Parent != nullptr) Parent->AddChild(this);
	pScene = Sce;
	m_pShaderManager = Sce->GetShaderManager();

}



void Object::AddInstance(ObjectInstance* TsetInfo)
{
	if (TsetInfo == nullptr) return;

	InstanceList.push_back(TsetInfo);
	//행렬 버퍼 크기 증가
	MatrixList.push_back(glm::mat4());
	AddInstanceCallBack();
}
void Object::Update(GLfloat dtime)
{
	//여러번 업데이트 방지
	if (m_bUpdated) return;
	//리소스 (모델공간) 업데이트 -> 초기값???
	glm::mat4  mTrans = glm::translate(glm::vec3(vPos));

	//회전의 경우 짐벌락등 여러 문제 발생가능
	//나중에 쿼터니언이나 Local 축 기준 회전 이용 고려
	glm::mat4  mRotate = glm::rotate(glm::mat4(), vRot.x, glm::vec3(1.f, 0.f, 0.f));
	mRotate = glm::rotate(mRotate, vRot.y, glm::vec3(0.f, 1.f, 0.f));
	mRotate = glm::rotate(mRotate, vRot.z, glm::vec3(0.f, 0.f, 1.f));

	glm::mat4  mScale = glm::scale(vScale);

	//S > R > T OpenGL에서는 뒤에서부터 읽음
	TransformMat = mTrans * mRotate * mScale;
	//TransformMat = glm::mat4();

	//인스턴스들 업데이트
	//for (int i = 0; i < InstanceList.size(); i++)
	//{
	//	InstanceList[i]->Update(dtime);
	//}
	m_bUpdated = true;

	//자식 오브젝트 업데이트
	//for (int i = 0; i < ChildList.size(); i++)
	//{
	//	ChildList[i]->Update(dtime);
	//}
}
void Object::Render()
{
	MyShader* ThisShader = m_pShaderManager->ApplyShaderByName(ForwardShaderName);

	if (ThisShader)	ShaderParamInit(ThisShader);

	

	

	if (m_pModel != nullptr)
	{
		if (GetInstanceNum() == 0) m_pModel->Render(MatLocArray);
		else
		{
			InstanceDataSetting();
			m_pModel->RenderInstance(GetInstanceNum(), MatLocArray);

		}
	}
	// 그렸으므로 업데이트필요
	m_bUpdated = false;

	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->Render();
	}


}

void Object::RenderGeoPass()
{
	MyShader* ThisShader = m_pShaderManager->ApplyShaderByName(GeoShaderName);

	if (ThisShader)
	
	{
		GeoPassInit(ThisShader);
	
	}
	

	if (m_pModel != nullptr)
	{
		if ( GetInstanceNum() == 0) m_pModel->Render(MatLocArray);
		else
		{
			InstanceDataSetting();
			m_pModel->RenderInstance( GetInstanceNum(), MatLocArray);

		}
	}
	m_bUpdated = false;


	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->RenderGeoPass();
		//Children[i]->Render();
	}

}
void Object::RenderShadowPass()
{
	MyShader* ThisShader = m_pShaderManager->ApplyShaderByName(ShadowShaderName);

	if (ThisShader)
	{
		ShadowPassInit(ThisShader);

	}
	
	if (GetInstanceNum() == 0) m_pModel->Render();
	else
	{
		InstanceDataSetting();
		m_pModel->RenderInstance(GetInstanceNum());

	}
	
	m_bUpdated = false;

	
	for (GLuint i = 0; i<ChildList.size(); i++)
	{
		ChildList[i]->RenderShadowPass();
		//Children[i]->Render();
	}
}


void Object::InstanceDataSetting()
{
	m_pModel->SetInstanceBufferData(GetInstanceMatrixData(), 0);

}

void Object::GeoPassInit(MyShader* ManagedShader)
{
	MyShader* ThisShader;
	if (ManagedShader == nullptr) return;
	else ThisShader = ManagedShader;

	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 M;

	//if (mParent == nullptr) M = TransformMat;
	//else  M = TransformMat*mParent->GetModelMat();
	M = TransformMat;

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;

	ThisShader->SetUniformMatrix4fv("WVP", glm::value_ptr(MVP));
	ThisShader->SetUniformMatrix4fv("World", glm::value_ptr(M));

	MatLocArray[0] = glGetUniformLocation(ThisShader->GetShaderProgram(), "material.diffuse");
	MatLocArray[1] = glGetUniformLocation(ThisShader->GetShaderProgram(), "material.amdient");
	MatLocArray[2] = glGetUniformLocation(ThisShader->GetShaderProgram(), "material.specular");
	MatLocArray[3] = glGetUniformLocation(ThisShader->GetShaderProgram(), "material.shininess");

	////////InctanceData 셋팅
	

}
void Object::ShaderParamInit(MyShader* ManagedShader)
{
	MyShader* ThisShader;
	if (ManagedShader == nullptr) return;
	else ThisShader = ManagedShader;

	glm::mat4 V = pScene->GetVMatrix();
	glm::mat4 VP = pScene->GetVPMatrix();
	glm::mat4 M;


	//Dir Light 정보 보내기

	DirLight* pDirLit = pScene->GetDirectionalLight();
	glm::vec4 DirLightPos = pDirLit->GetPos();
	//glm::vec4 DirLightPos = glm::vec4(0,-15,0,1);
	ThisShader->SetUniform4fv("gDirLight.LPos", glm::value_ptr(DirLightPos));

	glm::vec4 paramDiff = glm::vec4(pDirLit->GetDif(), 1);
	glm::vec4 paramAmbi = glm::vec4(pDirLit->GetAmb(), 1);
	glm::vec4 paramSpec = glm::vec4(pDirLit->GetSpec(), 1);


	ThisShader->SetUniform4fv("gDirLight.LDiff", glm::value_ptr(paramDiff));
	ThisShader->SetUniform4fv("gDirLight.LAmbi", glm::value_ptr(paramAmbi));
	ThisShader->SetUniform4fv("gDirLight.LSpec", glm::value_ptr(paramSpec));

	glm::vec4 CameraPos = pScene->GetCurrentCamPos();
	ThisShader->SetUniform4fv("gEyeWorldPos", glm::value_ptr(CameraPos));


	//if (mParent == nullptr) M = TransformMat;
	//else  M = TransformMat*mParent->GetModelMat();
	M = TransformMat;

	glm::mat4 MV = V*M;
	glm::mat4 MVP = VP*M;
	ThisShader->SetUniformMatrix4fv("MV", glm::value_ptr(MV));
	ThisShader->SetUniformMatrix4fv("MVP", glm::value_ptr(MVP));
	ThisShader->SetUniformMatrix4fv("V", glm::value_ptr(V));
	ThisShader->SetUniformMatrix4fv("M", glm::value_ptr(M));
	ThisShader->SetUniformMatrix4fv("VP", glm::value_ptr(VP));

	// 빛 정보 UiformBlock 쉐이더 전송 
	LightList* DataforShader = pScene->GetLightSrouceArray();
	GLuint Size = DataforShader->Count * sizeof(PaddingLight);
	//meshes[i]->UpdateUBO(DataforShader, Size+ sizeof(GLuint), 0);
	UpdateUBO(DataforShader, sizeof(GLuint), 0);

	// std140 stride 16
	UpdateUBO(DataforShader, Size, 12);


	////////InctanceData 셋팅
	m_pModel->SetInstanceBufferData(GetInstanceMatrixData(), 0);

	MatLocArray[0] = glGetUniformLocation(ThisShader->GetShaderProgram(), "material.diffuse");
	MatLocArray[1] = glGetUniformLocation(ThisShader->GetShaderProgram(), "material.amdient");
	MatLocArray[2] = glGetUniformLocation(ThisShader->GetShaderProgram(), "material.specular");
	MatLocArray[3] = glGetUniformLocation(ThisShader->GetShaderProgram(), "material.shininess");


}
void Object::ShadowPassInit(MyShader* ManagedShader)
{

}

void Object::AddChild(Object* pChild)
{
	ChildList.push_back(pChild);
}
Node* Object::GetRoot()
{
	return pRoot;
}
int Object::GetInstanceNum()
{
	return InstanceList.size();
}
glm::mat4* Object::GetInstanceMatrixData()
{
	if (MatrixList.size() != InstanceList.size()) printf("오류 :인스턴스와 행렬버퍼의 크기가 다름\n");
	//값만 복사해놓고 넘겨줌
	for (int i = 0; i < MatrixList.size(); i++)
	{
		MatrixList[i] = InstanceList[i]->GetMat();
	}
	return 	MatrixList.data();

}

glm::mat4  Object::GetModelMat()
{

	//return (mParent == nullptr) ? TransformMat : TransformMat*mParent->GetModelMat();
	return TransformMat;
}

void Object::AddUBO(void* Data, GLuint Size, const char* BlockName, GLuint* Offset, MyShader* pshad)
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
		"List[0].LAmbi" , "List[0].LSpec" , "List[0].LAttnuation" };
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
void Object::UpdateUBO(void* Data, GLuint Size, GLuint Offset)
{
	if (ubo <= 0) return;

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, Offset, Size, Data);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);
	//LightList* d = (LightList*)Data;
}
void Object::SetPosition(float x, float y, float z)
{
	vPos.x = x; vPos.y = y; vPos.z = z;
}