#include "CommonHeader.h"
#include "ProgramManager.h"
#include "myShader.h"




MyShader::MyShader()
{
}
MyShader::MyShader(const char* file_vert, const char* file_frag,
	const char* file_tesc, const char* file_tese,
	const char* file_geom)
{
	build_program_from_files(file_vert, file_frag, file_tesc, file_tese, file_geom);
}


bool MyShader::check_compile_status(GLuint handle)
{
	GLint	status;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint	len;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &len);
		GLchar  *buf_log = new GLchar[len + 1];
		glGetShaderInfoLog(handle, len, NULL, buf_log);
		std::cerr << "Compilation failed: " << buf_log << std::endl;
		delete[] buf_log;
	}
	return (status == GL_TRUE);
}

GLuint MyShader::compile_shader(GLenum type, const char* source)
{
	GLuint	handle = glCreateShader(type);
	if (!handle)
	{
		std::cerr << "Shader object creation failed." << std::endl;
		return handle;
	}
	glShaderSource(handle, 1, &source, NULL);
	glCompileShader(handle);
	if (!check_compile_status(handle))
	{
		std::cerr << "Shader compilation failed." << std::endl;
		return 0;
	}
	return handle;
}

bool MyShader::check_link_status(GLuint handle)
{
	GLint	status;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint	len;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &len);
		GLchar	*buf_log = new GLchar[len + 1];
		glGetProgramInfoLog(handle, len, NULL, buf_log);
		std::cerr << "Linking failed: " << buf_log << std::endl;
		delete[] buf_log;
	}
	return (status == GL_TRUE);
}

GLuint MyShader::build_program(
	const char* src_vert,
	const char* src_frag,
	const char* src_tesc,
	const char* src_tese,
	const char* src_geom )
{
	GLuint	h_vert = compile_shader(GL_VERTEX_SHADER, src_vert);
	if (!h_vert)
	{
		std::cerr << "Error while compiling the vertex shader" << std::endl;
		return 0;
	}
	GLuint	h_frag = compile_shader(GL_FRAGMENT_SHADER, src_frag);
	if (!h_frag)
	{
		std::cerr << "Error wihle compiling the fragment shader" << std::endl;
		return 0;
	}

	GLuint  h_tesc = 0;
	if (src_tesc)
	{
		h_tesc = compile_shader(GL_TESS_CONTROL_SHADER, src_tesc);
		if (!h_tesc)
		{
			std::cerr << "Error wihle compiling the TCS (Tess Control Shader)" << std::endl;
			return 0;
		}
	}
	GLuint  h_tese = 0;
	if (src_tese)
	{
		//      printf("%s\n", src_tese);
		h_tese = compile_shader(GL_TESS_EVALUATION_SHADER, src_tese);
		if (!h_tese)
		{
			std::cerr << "Error wihle compiling the TES (Tess Evaluation Shader)" << std::endl;
			return 0;
		}
	}
	GLuint  h_geom = 0;
	if (src_geom)
	{
		h_geom = compile_shader(GL_GEOMETRY_SHADER, src_geom);
		if (!h_geom)
		{
			std::cerr << "Error wihle compiling the geometry shader" << std::endl;
			return 0;
		}
	}

	mProgram = glCreateProgram();
	if (!mProgram)
	{
		std::cerr << "Program object creation failed." << std::endl;
		return mProgram;
	}
	glAttachShader(mProgram, h_vert);
	glAttachShader(mProgram, h_frag);
	if (h_tesc)  glAttachShader(mProgram, h_tesc);
	if (h_tese)  glAttachShader(mProgram, h_tese);
	if (h_geom)  glAttachShader(mProgram, h_geom);

	glLinkProgram(mProgram);
	if (!check_link_status(mProgram))	return 0;

	return mProgram;
}

std::string MyShader::get_file_contents(const char *filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

GLuint MyShader::build_program_from_files( 
	const char* file_vert,
	const char* file_frag,
	const char* file_tesc,
	const char* file_tese,
	const char* file_geom )
{
	std::string src_vert = get_file_contents(file_vert);
	std::string src_frag;
	std::string src_tesc;
	std::string src_tese;
	std::string src_geom;

	if (file_frag)   src_frag = get_file_contents(file_frag);
	if (file_tesc)   src_tesc = get_file_contents(file_tesc);
	if (file_tese)   src_tese = get_file_contents(file_tese);
	if (file_geom)   src_geom = get_file_contents(file_geom);
	return build_program(src_vert.c_str(),
		src_frag.size() ? src_frag.c_str() : NULL,
		src_tesc.size() ? src_tesc.c_str() : NULL,
		src_tese.size() ? src_tese.c_str() : NULL,
		src_geom.size() ? src_geom.c_str() : NULL );
}

GLuint MyShader::GetShaderProgram()
{
	return mProgram;
}
void MyShader::ApplyShader()
{
	
	ProgramManager* s = ProgramManager::GetInstance();
		s->SetCurrentShader(this);

	glUseProgram(mProgram);
}

void MyShader::SetUniform4fv(const char* Var, GLfloat* value, GLuint count)
{
	glUseProgram(mProgram);
	glUniform4fv(glGetUniformLocation(mProgram, Var), count, value);

}


void MyShader::SetUniform3fv(const char* Var, GLfloat* value, GLuint count)
{
	glUseProgram(mProgram);
	glUniform3fv(glGetUniformLocation(mProgram, Var), count, value);

}

void MyShader::SetUniformMatrix4fv(const char* Var, GLfloat* value,  GLuint count)
{
	glUseProgram(mProgram);
	glUniformMatrix4fv(glGetUniformLocation(mProgram, Var), count, GL_FALSE, value);
}
void MyShader::SetUniformMatrix3fv(const char* Var, GLfloat* value, GLuint count)
{
	glUseProgram(mProgram);
	glUniformMatrix3fv(glGetUniformLocation(mProgram, Var), count, GL_FALSE, value);
}

void MyShader::SetUniform1f(const char* Var, GLfloat value)
{
	glUseProgram(mProgram);
	glUniform1f(glGetUniformLocation(mProgram, Var),  value);
}
void MyShader::SetUniform1i(const char* Var, GLint value)
{
	glUseProgram(mProgram);
	glUniform1i(glGetUniformLocation(mProgram, Var), value);
}


////////////////////////////////////////////////////////////////
/////////////////////쉐이더 매니져
ShaderManager::ShaderManager()
{
	//ChannelCount = 2;
	
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTexture);
	printf("Max Texture Unit : %d\n", MaxTexture);
	m_pbUseTextrueUnit = new bool[MaxTexture] {false,};
}

GLuint ShaderManager::GetChannelID()
{
	for (int i = 0; i < MaxTexture;i++)
	{
		if (m_pbUseTextrueUnit[i] == false)
		{
			m_pbUseTextrueUnit[i] = true;
			return i;
		}
	}

	//비어있는 텍스쳐 유닛 없음
	printf("비어있는 텍스쳐 유닛 없음\n");
	return -1;
}
void ShaderManager::LockShader(MyShader* pShader)
{
	m_IsLockShader = true;
	m_pLockShader = pShader;
}
void ShaderManager::ReleaseLock()
{
	m_IsLockShader = false;
}
void ShaderManager::ReleaseChannel(int Channel)
{
	m_pbUseTextrueUnit[Channel] = true;
}

void ShaderManager::SetCurrentShader(MyShader* Cur)
{
	CurrentShader = Cur;
}
MyShader* ShaderManager::GetCurrentShader()
{
	return CurrentShader;
}