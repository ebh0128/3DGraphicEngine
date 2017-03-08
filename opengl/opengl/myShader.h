#pragma once
class MyShader
{
private:
	GLuint mProgram;

public:
	MyShader();
	MyShader( const char* file_vert, const char* file_frag,
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


	void SetUniformMatrix4fv(const char* Var, GLfloat* value , GLuint count=1);
	void SetUniformMatrix3fv(const char* Var, GLfloat* value, GLuint count = 1);
	void SetUniformMatrix2fv(const char* Var, GLfloat* value, GLuint count = 1);

	void SetUniform1f(const char* Var, GLfloat value);
	void SetUniform1i(const char* Var, GLint value);
	GLuint GetShaderProgram();
	void ApplyShader();
};

//싱글톤 클래스 아님 전체매니져 하나에서만 사용
class ShaderManager
{
private:
	//진입점에서 초기화해주면 좋음
	MyShader* CurrentShader;
	GLuint ChannelCount;

	//텍스쳐 유닛이 사용하고 있는지 여부
	bool* m_pbUseTextrueUnit;
	GLint MaxTexture;

	//쉐이더 락(강제로 특정 쉐이더를 적용할 때 사용)
	bool m_IsLockShader;
	MyShader* m_pLockShader;
public:
	//초기화는 여기서
	ShaderManager();

	void SetCurrentShader(MyShader* Cur);
	GLuint GetChannelID();
	
	bool CheckLock() { return m_IsLockShader; }
	//쉐이더 잠그기
	void LockShader(MyShader* pShader);

	//잠금 해제
	void ReleaseLock();

	//텍스쳐 채널해제 현재는 플래그만 해제
	void ReleaseChannel(int Channel);
	MyShader* GetCurrentShader();
	

};