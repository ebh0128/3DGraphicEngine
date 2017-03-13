#pragma once

class AssimpModelNode : public Node
{
protected:
	bool IsRootNode;
	bool IsTextured;

	GLuint MainTextUnit;
public:

	AssimpModelNode();

	//자식 노드생성
	AssimpModelNode(Node* parent, SceneGL* Scene);
	
	//루트 노드 생성(Assimp Read)
	AssimpModelNode(Node* parent, SceneGL* Scene ,std::string FilePath, std::string FileName);
	void InitAssimpNode(aiNode* pNode , const aiScene* pScene, std::string FilePath);
	void SetNoTexture();
	void SetScale(glm::vec3 scale);
	void Update(GLfloat dtime);
	void Render();


	void ShaderParamInit();
	void GeoPassInit();
	//void LitPassInit();
	void ShadowPassInit();

};