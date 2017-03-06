
///////////////////////////////////////////////////////////////////////
//
// triangles.cpp
//
// modified by Minho Kim (minhokim@uos.ac.kr) on 10 October 2016
//
// for OpenGL 3.3 and above
//
///////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <GLUT/GLUT.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif
#ifndef BUFFER_OFFSET   
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif
#define USE_DSA 0   // DSA requires OpenGL version >= 4.5
GLuint  vao;
GLuint  vbo;
GLuint  h_prog;
static GLuint   att_idx = 0;
#if USE_DSA
static GLuint   att_binding = 7;
#endif



void init(void)
{
	GLfloat vertices[6][2] = {
		{ -0.90, -0.90 }, // Triangle 1
		{ 0.85, -0.90 },
		{ -0.90,  0.85 },
		{ 0.90, -0.85 }, // Triangle 2
		{ 0.90,  0.90 },
		{ -0.85,  0.90 }
	};

	const char  src_vert[] = {
		"#version 330 core  \n"
		"layout(location = 0) in vec4 aPosition;    \n"
		"void   \n"
		"main() \n"
		"{  \n"
		"\t gl_Position = aPosition;    \n"
		"}  \n"
	};
	const char src_frag[] = {
		"#version 330 core\n"
		"out vec4 fColor;\n"
		"void\n"
		"main()\n"
		"{\n"
		"\tfColor = vec4(0.0, 0.0, 1.0, 1.0);\n"
		"}\n"
	};
	const char  *src;
	GLuint  h_vert = glCreateShader(GL_VERTEX_SHADER);
	src = src_vert;
	glShaderSource(h_vert, 1, &src, NULL);
	glCompileShader(h_vert);

	GLuint  h_frag = glCreateShader(GL_FRAGMENT_SHADER);
	src = src_frag;
	glShaderSource(h_frag, 1, &src, NULL);
	glCompileShader(h_frag);

	h_prog = glCreateProgram();
	glAttachShader(h_prog, h_vert);
	glAttachShader(h_prog, h_frag);
	glLinkProgram(h_prog);

#if USE_DSA
	GLint   value;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &value);
	printf("GL_MAX_VERTEX_ATTRIB_BINDINGS=%d\n", value);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao); // bug. This should NOT be necessary for DSA calls.
							// Currently, the following calls generate "GL_INVALID_OPERATION" error without this call.
	glEnableVertexArrayAttrib(vao, att_idx);
	glVertexArrayAttribFormat(vao, att_idx, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, att_binding, vbo, 0, sizeof(GLfloat) * 2);   // "stride" cannot be 0.
	glVertexArrayAttribBinding(vao, att_idx, att_binding);

	glNamedBufferDataEXT(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);  // glNamedBufferData generates a segmentation fault (10 Oct 2016)
#else

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// The order of bindings of VBO & VAO does not matter,
	// but they both should be bound before calling glVertexAttribPointer!
	glVertexAttribPointer(att_idx, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(att_idx);
#endif // USE_DSA

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(h_prog);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
#ifdef  __APPLE__
	glutInitDisplayMode(GLUT_RGBA | GLUT_3_2_CORE_PROFILE);
	glutCreateWindow(argv[0]);
#else
	glutInitDisplayMode(GLUT_RGBA);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitContextProfile(GLUT_FORWARD_COMPATIBLE);
	glutCreateWindow(argv[0]);
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
#endif
	init();
	glutDisplayFunc(display);
	glutMainLoop();
}

