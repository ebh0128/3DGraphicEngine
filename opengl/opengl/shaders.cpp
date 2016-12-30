bool check_compile_status(GLuint handle)
{
	GLint	status;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint	len;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &len);
		GLchar  *buf_log = new GLchar [len+1];
		glGetShaderInfoLog(handle, len, NULL, buf_log);
		std::cerr << "Compilation failed: " << buf_log << std::endl;
		delete[] buf_log;
	}
	return (status==GL_TRUE);
}

GLuint compile_shader(GLenum type, const char* source)
{
	GLuint	handle = glCreateShader(type);
	if(!handle)
	{
		std::cerr << "Shader object creation failed." << std::endl;
		return handle;
	}
	glShaderSource( handle, 1, &source, NULL );
	glCompileShader( handle );
	if(!check_compile_status(handle))
	{
		std::cerr << "Shader compilation failed." << std::endl;
		return 0;
	}
	return handle;
}

bool check_link_status(GLuint handle)
{
	GLint	status;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint	len;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &len);
		GLchar	*buf_log = new GLchar[len+1];
		glGetProgramInfoLog(handle, len, NULL, buf_log);
		std::cerr << "Linking failed: " << buf_log << std::endl;
		delete[] buf_log;
	}
	return (status==GL_TRUE);
}

GLuint build_program(const char* src_vert, const char* src_frag)
{
	GLuint	h_vert = compile_shader(GL_VERTEX_SHADER, src_vert);
	if(!h_vert)
	{
		std::cerr << "Error while compiling the vertex shader" << std::endl;
		return 0;
	}
	GLuint	h_frag = compile_shader(GL_FRAGMENT_SHADER, src_frag);
	if(!h_frag)
	{
		std::cerr << "Error wihle compiling the fragment shader" << std::endl;
		return 0;
	}

	GLuint h_prog = glCreateProgram();
	if(!h_prog)
	{
		std::cerr << "Program object creation failed." << std::endl;
		return h_prog;
	}
	glAttachShader( h_prog, h_vert );
	glAttachShader( h_prog, h_frag );
	glLinkProgram( h_prog );
	if(!check_link_status(h_prog))	return 0;

	return h_prog;
}

// original source:
// http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
//
std::string get_file_contents(const char *filename)
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

GLuint build_program_from_files(const char* file_vert, const char* file_frag)
{
	std::string src_vert = get_file_contents(file_vert);
	std::string src_frag = get_file_contents(file_frag);
	return build_program(src_vert.c_str(), src_frag.c_str());
}


