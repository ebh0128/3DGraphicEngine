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

//�̱��� Ŭ���� �ƴ� ��ü�Ŵ��� �ϳ������� ���
class PassShaderObject;

class ShaderManager
{
protected:
	std::vector<PassShaderObject*> m_ShaderObjectList;
	
public:
	//�ʱ�ȭ�� ���⼭
	ShaderManager();

	//���� ��û ���� ������ ���������ʰ� �̸� ��ȯ
	//������ ������ �̸���ȯ
	std::string CreateShader(const char* file_vert, const char* file_frag,
		const char* file_tesc = NULL, const char* file_tese = NULL,
		const char* file_geom = NULL);

	//���̴� ������Ʈ�� �̸����� ã�� �޼ҵ�
	PassShaderObject* FindPassShader(std::string InputName);
	MyShader* ApplyShaderByName(std::string shaderName);


};