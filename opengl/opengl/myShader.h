#pragma once
class MyShader
{
private:
	GLuint mProgram;

public:
	MyShader();
	MyShader(const char* src_vert, const char* src_frag);

	//Shader 빌드
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

	void SetUniformMatrix4fv(const char* Var, GLfloat* value , GLuint count=1);
	void SetUniformMatrix3fv(const char* Var, GLfloat* value, GLuint count = 1);
	void SetUniform1f(const char* Var, GLfloat value);
	void SetUniform1i(const char* Var, GLint value);
	GLuint GetShaderProgram();
	void ApplyShader();
};

//쉐이더의 현재 상태를 알수 있는 싱글톤 클래스 ver0.1
//활성화된 쉐이더가 뭔지만 저장
//추후 쉐이더가 많아지면 여기 저장후 uniform만 바꾸는식으로 변경 예정
class ShaderManager
{
private:
	//진입점에서 초기화해주면 좋음
	static ShaderManager* pInstance;
	
	//초기화는 여기서
	ShaderManager();


	MyShader* CurrentShader;
	GLuint ChannelCount;


public:
	static ShaderManager* GetInstance()
	{
		if (pInstance == nullptr) pInstance = new ShaderManager();
		return pInstance;
	}
	void SetCurrentShader(MyShader* Cur);
	GLuint GetChannelID();
	MyShader* GetCurrentShader();
	

};