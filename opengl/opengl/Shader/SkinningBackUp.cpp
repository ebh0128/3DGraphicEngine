#include "CommonHeader.h"
#include "SkinningModel.h"




SkinningModel::SkinningModel() :Model()
{

}

void SkinningModel::BoneTransform(float TimeInSecons, std::vector< glm::mat4>& Transforms)
{
	glm::mat4 Identity = glm::mat4();

	float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSecons * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);
	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	Transforms.resize(m_NumBones);

	for (GLuint i = 0; i < m_NumBones; i++)
	{
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}

}

void SkinningModel::CreateAssimpModel(const aiScene* pAssimpScene, std::string FilePath)
{

	for (int i = 0; i < pAssimpScene->mNumMeshes; i++)
	{
		const aiMesh* AssimpMesh = pAssimpScene->mMeshes[i];

		MeshEntry* NewMesh = new MeshEntry(pAssimpScene, AssimpMesh , true);
		NewMesh->SetBaseVertex(m_NumVertices);


		m_NumVertices += AssimpMesh->mNumVertices;
		//텍스쳐 찾기
		aiMaterial* aiMat = pAssimpScene->mMaterials[AssimpMesh->mMaterialIndex];
		aiString TexturePath;
		//텍스쳐의 상대경로를 알아냄 (Object 의 경로로부터의)
		aiReturn ret = aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &TexturePath);
		if (ret == AI_SUCCESS)
		{
			
			char* TPath = TexturePath.data;
			std::string TextureName = TPath;
			std::istringstream iss(TextureName);
			std::string token;
			while (getline(iss, token, '/'))
			{}
			TextureName = '/' + token;
			std::string sDir = FilePath + TextureName;
			Texture* MainTextue = new Texture(sDir.c_str(), m_MainTextureUnitNum);
			NewMesh->AddTexture(MainTextue);

		}
		AddMesh(NewMesh);
	}

	//뼈버퍼 다 만듬(버퍼 크기를 미리 할당 못하기때문에 따로 빼야됨....)
	std::vector<VertexBoneData> TempBones;
	TempBones.resize(m_NumVertices);
	for (int i = 0; i < pAssimpScene->mNumMeshes; i++)
	{
		const aiMesh* pMesh = pAssimpScene->mMeshes[i];
		LoadBones(i, pMesh, TempBones);
	}
	
	for (int i = 0; i < m_MeshList.size(); i++)
	{
		m_MeshList[i]->AddBoneData(TempBones);
	}

	MakeInstancingBuffer();
}


void SkinningModel::CreateModelFromFile(std::string FilePath, std::string FileName)
{
	std::string FullPath = FilePath + FileName;
	m_pScene = m_Importer.ReadFile(FullPath,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices | //aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals);

	if (m_pScene)
	{
		aiMatrix4x4 NodeTrans = m_pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform = AssimpToGlmMatrix(NodeTrans, m_GlobalInverseTransform);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
	}

	CreateAssimpModel(m_pScene, FilePath);

}

void SkinningModel::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{

	if (pNodeAnim->mNumScalingKeys == 1)
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	GLuint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	GLuint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}
void SkinningModel::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) 
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	GLuint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	GLuint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}
void SkinningModel::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* aiNodeAnim)
{
	//1개면 보간 필요없음
	if (aiNodeAnim->mNumPositionKeys == 1)
	{
		Out = aiNodeAnim->mPositionKeys[0].mValue;
		return;
	}
	GLuint PositionIndex = FindPosition(AnimationTime, aiNodeAnim);
	GLuint NextPositionIndex = PositionIndex + 1;
	assert(NextPositionIndex < aiNodeAnim->mNumPositionKeys);

	float DeltaTime = (float)(aiNodeAnim->mPositionKeys[NextPositionIndex].mTime - aiNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)aiNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

	assert(Factor >= 0.0f && Factor <= 1.0f);

	const aiVector3D& Start = aiNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = aiNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;

	Out = Start + Factor * Delta;
}
GLuint SkinningModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (GLuint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
GLuint SkinningModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (GLuint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
GLuint SkinningModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// 다음것과 보간하기에 마지막 키는 의미 없음
	for (GLuint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	//이까지 오면 당연히 오류
	assert(0);
	return 0;
}
const aiNodeAnim* SkinningModel::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (GLuint i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim= pAnimation->mChannels[i];
		if (std::string(pNodeAnim->mNodeName.data) == NodeName)
		{
			return pNodeAnim;
		}
	}

	return NULL;
}
void SkinningModel::ReadNodeHeirarchy(float AnimationTime, const aiNode*pNode, const glm::mat4& ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	glm::mat4 NodeTransform;
	aiMatrix4x4 AssimTransform;
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
	
	//각 성분 보간
	if (pNodeAnim)
	{
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 mSclaing = glm::scale(glm::vec3(Scaling.x , Scaling.y , Scaling.z));
		//glm::mat4 mSclaing = glm::scale(glm::vec3(1, 1, 1));


		aiQuaternion RotQ;
		CalcInterpolatedRotation(RotQ, AnimationTime, pNodeAnim);
		aiMatrix4x4 aiRotation = aiMatrix4x4(RotQ.GetMatrix());
		glm::mat4 mRotation  = glm::mat4();
		AssimpToGlmMatrix(aiRotation, mRotation);

		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 mTranslation = glm::translate(glm::vec3(Translation.x, Translation.y, Translation.z));
		
		NodeTransform = mTranslation * mRotation * mSclaing;
			//NodeTransform = glm::transpose(NodeTransform);
			int a = 10;
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransform;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
	{
		GLuint BoneIndex = m_BoneMapping[NodeName];
		glm::mat4 mBoneOffset = AssimpToGlmMatrix(m_BoneInfo[BoneIndex].BoneOffset, mBoneOffset);
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation*mBoneOffset;
		int a = 10;
	}

	for (GLuint i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}
void SkinningModel::LoadBones(GLuint MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones)
{
	for (int i = 0; i < paiMesh->mNumBones; i++)
	{
		GLuint BoneIndex = 0;
		std::string BoneName(paiMesh->mBones[i]->mName.data);

		//새로운 본일때 추가
		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
		{
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = paiMesh->mBones[i]->mOffsetMatrix;
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (int j = 0; j < paiMesh->mBones[i]->mNumWeights; j++)
		{
			GLuint VertexID = m_MeshList[MeshIndex]->GetBaseVertex() + paiMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = paiMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}