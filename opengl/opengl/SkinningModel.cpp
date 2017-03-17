#include "CommonHeader.h"
#include "SkinningModel.h"

typedef unsigned int uint;
using namespace std;

void AssimpToGlmMatrix(aiMatrix4x4& ai, glm::mat4& Mat)
{
	Mat[0][0] = ai.a1; Mat[0][1] = ai.b1; Mat[0][2] = ai.c1; Mat[0][3] = ai.d1;
	Mat[1][0] = ai.a2; Mat[1][1] = ai.b2; Mat[1][2] = ai.c2; Mat[1][3] = ai.d2;
	Mat[2][0] = ai.a3; Mat[2][1] = ai.b3; Mat[2][2] = ai.c3; Mat[2][3] = ai.d3;
	Mat[3][0] = ai.a4; Mat[3][1] = ai.b4; Mat[3][2] = ai.c4; Mat[3][3] = ai.d4;

}

void Matrix4fToGlmMatrix(Matrix4f& InM, glm::mat4& Mat)
{
	Mat[0][0] = InM.m[0][0]; Mat[0][1] = InM.m[1][0]; Mat[0][2] = InM.m[2][0]; Mat[0][3] = InM.m[3][0];
	Mat[1][0] = InM.m[0][1]; Mat[1][1] = InM.m[1][1]; Mat[1][2] = InM.m[2][1]; Mat[1][3] = InM.m[3][1];
	Mat[2][0] = InM.m[0][2]; Mat[2][1] = InM.m[1][2]; Mat[2][2] = InM.m[2][2]; Mat[2][3] = InM.m[3][2];
	Mat[3][0] = InM.m[0][3]; Mat[3][1] = InM.m[1][3]; Mat[3][2] = InM.m[2][3]; Mat[3][3] = InM.m[3][3];

}





SkinningModel::SkinningModel() :Model()
{

}

void SkinningModel::BoneTransform(float TimeInSecons, std::vector< glm::mat4>& Transforms)
{
	glm::mat4 Identity = glm::mat4();
	Matrix4f m4Identity;
	m4Identity.InitIdentity();
	
	float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSecons * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);

//	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, m4Identity);

	Transforms.resize(m_NumBones);

	for (GLuint i = 0; i < m_NumBones; i++)
	{
		//transpos
		Matrix4fToGlmMatrix(m_BoneInfo[i].m4FinalTransformation, m_BoneInfo[i].FinalTransformation);
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	//	Transforms[i] = m_GlobalInverseTransform;
	}
	

}

void SkinningModel::CreateAssimpModel(const aiScene* pAssimpScene, std::string FilePath)
{
	uint NumIndices = 0;
	for (int i = 0; i < pAssimpScene->mNumMeshes; i++)
	{
		const aiMesh* AssimpMesh = pAssimpScene->mMeshes[i];

		MeshEntry* NewMesh = new MeshEntry(pAssimpScene, AssimpMesh , true);
		NewMesh->SetBaseVertex(m_NumVertices);
		NewMesh->SetBaseIndex(NumIndices);
		
		NumIndices += AssimpMesh->mNumFaces * 3;
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
		aiProcess_GenSmoothNormals |
		// aiProcess_FlipUVs |
		 aiProcess_JoinIdenticalVertices 
	);

	if (m_pScene)
	{
		aiMatrix4x4 NodeTrans = m_pScene->mRootNode->mTransformation;
		m_m4GlobalInverseTransform = NodeTrans;
		m_m4GlobalInverseTransform = m_m4GlobalInverseTransform.Inverse();
		AssimpToGlmMatrix(NodeTrans, m_GlobalInverseTransform);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
	}

	CreateAssimpModel(m_pScene, FilePath);

}

void SkinningModel::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{

	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
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
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}
void SkinningModel::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	//1개면 보간 필요없음
	if (pNodeAnim->mNumPositionKeys == 1) 
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;

}
GLuint SkinningModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
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

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
GLuint SkinningModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
const aiNodeAnim* SkinningModel::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
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
	glm::mat4 mTranslation;
	glm::mat4 mRotation = glm::mat4();
	glm::mat4 mSclaing;
	
	if (pNodeAnim)
	{
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		mSclaing = glm::scale(glm::vec3(Scaling.x , Scaling.y , Scaling.z));
		//glm::mat4 mSclaing = glm::scale(glm::vec3(1, 1, 1));
		
		
		aiQuaternion RotQ;
		
		CalcInterpolatedRotation(RotQ, AnimationTime, pNodeAnim);
		mRotation  = glm::mat4();
		glm::quat glmRotQ = glm::quat(RotQ.w ,RotQ.x ,RotQ.y , RotQ.z);
		mRotation = glm::toMat4(glmRotQ);
		//AssimpToGlmMatrix(aiRotation, mRotation);
	

		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		mTranslation = glm::translate(glm::vec3(Translation.x, Translation.y, Translation.z));
		
		
		NodeTransform = mTranslation * mRotation * mSclaing;
		//NodeTransform = mSclaing * mRotation * mTranslation;

		int a = 10;
	}

	//glm::mat4 GlobalTransformation =  NodeTransform * ParentTransform;

	glm::mat4 GlobalTransformation =  ParentTransform * NodeTransform;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
	{
		GLuint BoneIndex = m_BoneMapping[NodeName];
		glm::mat4 mBoneOffset;
		AssimpToGlmMatrix(m_BoneInfo[BoneIndex].BoneOffset, mBoneOffset);
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation*mBoneOffset;
		//m_BoneInfo[BoneIndex].FinalTransformation = mBoneOffset * GlobalTransformation * m_GlobalInverseTransform;

		int a = 10;
	}

	for (GLuint i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}


void SkinningModel::ReadNodeHeirarchy(float AnimationTime, const aiNode*pNode, const Matrix4f& ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	Matrix4f NodeTransform = pNode->mTransformation;
	aiMatrix4x4 AssimTransform;
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	//각 성분 보간
	
	if (pNodeAnim)
	{
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		Matrix4f mScaling;
		mScaling.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);


		aiQuaternion RotQ;
		CalcInterpolatedRotation(RotQ, AnimationTime, pNodeAnim);
		Matrix4f mRotation = Matrix4f(RotQ.GetMatrix());
		//AssimpToGlmMatrix(aiRotation, mRotation);


		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		Matrix4f mTranslation;
		mTranslation.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

		NodeTransform = mTranslation * mRotation * mScaling;
		//NodeTransform = mSclaing * mRotation * mTranslation;

		int a = 10;
	}

	//glm::mat4 GlobalTransformation =  NodeTransform * ParentTransform;

	Matrix4f GlobalTransformation = ParentTransform * NodeTransform;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
	{
		GLuint BoneIndex = m_BoneMapping[NodeName];
		
		m_BoneInfo[BoneIndex].m4FinalTransformation = m_m4GlobalInverseTransform * GlobalTransformation*m_BoneInfo[BoneIndex].BoneOffset;
		//m_BoneInfo[BoneIndex].FinalTransformation = mBoneOffset * GlobalTransformation * m_GlobalInverseTransform;

		int a = 10;
	}

	for (GLuint i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}


void SkinningModel::LoadBones(GLuint MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		uint BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = m_MeshList[MeshIndex]->GetBaseVertex() + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}