#pragma once
class MyShader
{
private:
	GLuint mProgram;
	std::string ShaderName;

public:
	MyShader();
	MyShader(const char* file_vert, const char* file_frag,
		const char* file_tesc = NULL, const char* file_tese = NULL,
		const char* file_geom = NULL);

	GLuint build_program(const char* src_vert, const char* src_frag,
		const char* src_tesc = NULL, const char* src_tese = NULL,
		const char* src_geom = NULL);

	GLuint build_program_from_files(
		const char* file_vert, const char* file_frag,
		const char* file_tesc = NULL, const char* file_tese = NULL,
		const char* file_geom = NULL);

	bool check_compile_status(GLuint handle);
	GLuint compile_shader(GLenum type, const char* source);
	bool check_link_status(GLuint handle);

	std::string get_file_contents(const char *filename);

	void SetUniform4fv(const char* Var, GLfloat* value, GLuint count = 1);
	void SetUniform3fv(const char* Var, GLfloat* value, GLuint count = 1);
	void SetUniform2fv(const char* Var, GLfloat* value, GLuint count = 1);


	void SetUniformMatrix4fv(const char* Var, GLfloat* value, GLuint count = 1);
	void SetUniformMatrix3fv(const char* Var, GLfloat* value, GLuint count = 1);
	void SetUniformMatrix2fv(const char* Var, GLfloat* value, GLuint count = 1);

	void SetUniform1f(const char* Var, GLfloat value);
	void SetUniform1i(const char* Var, GLint value);
	GLuint GetShaderProgram();

	void GetName(char* retName);
	std::string GetNameString() { return ShaderName; }
	void SetNameString(std::string sName) { ShaderName = sName; }

	void ApplyShader();
};

//싱글톤 클래스 아님 전체매니져 하나에서만 사용
class PassShaderObject;

class Object;
class ShaderManager
{
protected:
	std::vector<PassShaderObject*> m_ShaderObjectList;
	
public:
	//초기화는 여기서
	ShaderManager();

	//생성 요청 만약 있으면 생성하지않고 이름 반환
	//없으면 생성후 이름반환
	std::string CreateShader(Object* pObj, const char* file_vert, const char* file_frag,
		const char* file_tesc = NULL, const char* file_tese = NULL,
		const char* file_geom = NULL);

	//쉐이더 오브젝트를 이름으로 찾는 메소드
	PassShaderObject* FindPassShader(std::string InputName);
	MyShader* ApplyShaderByName(std::string shaderName);


};