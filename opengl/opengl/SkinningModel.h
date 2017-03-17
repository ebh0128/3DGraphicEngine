#pragma once
#include "Mesh.h"


class SkinningModel : public Model
{
public:
	SkinningModel();


	GLuint NumBones() const
	{
		return m_NumBones;

	}
	void BoneTransform(float TimeInSecons, std::vector< glm::mat4>& Transforms);
	void CreateModelFromFile(std::string FilePath, std::string FileName);

protected:
	struct BoneInfo
	{
		aiMatrix4x4 BoneOffset;
		glm::mat4 FinalTransformation;
		Matrix4f m4FinalTransformation;
	};
	

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* aiNodeAnim);
	GLuint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	
	void ReadNodeHeirarchy(float AnimationTime, const aiNode*pNode, const glm::mat4& ParentTransform);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode*pNode, const Matrix4f& ParentTransform);


	
	void LoadBones(GLuint MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);

	bool InitFromScene(const aiScene* pScene, const std::string& Filename);

	void CreateAssimpModel(const aiScene* pAssimpScene, std::string FilePath);

	
	std::map<std::string, GLuint> m_BoneMapping;
	GLuint m_NumBones;
	std::vector<BoneInfo> m_BoneInfo;

	glm::mat4 m_GlobalInverseTransform;
	Matrix4f m_m4GlobalInverseTransform;
};