///////////////////////////////////////////////////////////////////////
//
// attribs.cpp
//
// modified by Minho Kim (minhokim@uos.ac.kr) on 22 September 2016
//
///////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <cstdlib>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <GLUT/GLUT.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#define	ATTRIBS_INTERLEAVED	1
#define	ATTRIBS_SEPARATE	2
#define	ATTRIBS_SEPARATE_TYPE	3
//#define	ATTRIBS_FORMAT	ATTRIBS_INTERLEAVED
//#define	ATTRIBS_FORMAT	ATTRIBS_SEPARATE
#define	ATTRIBS_FORMAT	ATTRIBS_SEPARATE_TYPE
#ifndef BUFFER_OFFSET	
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif

enum VAO_IDs { Triangles, NumVAOs };
#if	ATTRIBS_FORMAT==ATTRIBS_INTERLEAVED
enum Buffer_IDs { ArrayBuffer, NumBuffers };
#elif	(ATTRIBS_FORMAT==ATTRIBS_SEPARATE) || (ATTRIBS_FORMAT==ATTRIBS_SEPARATE_TYPE)
enum Buffer_IDs { PosBuffer, ColBuffer, NumBuffers };
#endif
enum Attrib_IDs { vPosition = 0, vColor };
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVertices = 6;

#include "shaders.cpp"

void init(void)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
#if	ATTRIBS_FORMAT==ATTRIBS_INTERLEAVED
	GLfloat vertices[NumVertices][5] = {
		{ -0.90, -0.90, 1, 0, 0 }, // Triangle 1
		{ 0.85, -0.90, 0, 1, 0 },
		{ -0.90, 0.85, 0, 0, 1 },
		{ 0.90, -0.85, 0, 1, 1 }, // Triangle 2
		{ 0.90, 0.90, 1, 0, 1 },
		{ -0.85, 0.90, 1, 1, 0 }
	};
#elif	ATTRIBS_FORMAT==ATTRIBS_SEPARATE
	GLfloat positions[NumVertices][2] = {
		{ -0.90, -0.90}, // Triangle 1
		{ 0.85, -0.90},
		{ -0.90, 0.85},
		{ 0.90, -0.85}, // Triangle 2
		{ 0.90, 0.90},
		{ -0.85, 0.90}
	};
	GLfloat colors[NumVertices][3] = {
		{1, 0, 0 }, // Triangle 1
		{0, 1, 0 },
		{0, 0, 1 },
		{0, 1, 1 }, // Triangle 2
		{1, 0, 1 },
		{1, 1, 0 }
	};
#elif	ATTRIBS_FORMAT==ATTRIBS_SEPARATE_TYPE
	GLfloat positions[NumVertices][2] = {
		{ -0.90, -0.90}, // Triangle 1
		{ 0.85, -0.90},
		{ -0.90, 0.85},
		{ 0.90, -0.85}, // Triangle 2
		{ 0.90, 0.90},
		{ -0.85, 0.90}
	};
	GLubyte colors[NumVertices][3] = {
		{255, 0, 0 }, // Triangle 1
		{0, 255, 0 },
		{0, 0, 255 },
		{0, 255, 255 }, // Triangle 2
		{255, 0, 255 },
		{255, 255, 0 }
	};
#endif
	glGenBuffers(NumBuffers, Buffers);
#if	ATTRIBS_FORMAT==ATTRIBS_INTERLEAVED
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
#elif	(ATTRIBS_FORMAT==ATTRIBS_SEPARATE) || (ATTRIBS_FORMAT==ATTRIBS_SEPARATE_TYPE)
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[PosBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ColBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
#endif

	GLuint h_prog = build_program_from_files("triangles_attribs.vert", "triangles_attribs.frag");

	glUseProgram(h_prog);
#if	ATTRIBS_FORMAT==ATTRIBS_INTERLEAVED
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), BUFFER_OFFSET(0));
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), BUFFER_OFFSET(2*sizeof(GLfloat)));
#elif	ATTRIBS_FORMAT==ATTRIBS_SEPARATE
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[PosBuffer]);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ColBuffer]);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
#elif	ATTRIBS_FORMAT==ATTRIBS_SEPARATE_TYPE
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[PosBuffer]);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ColBuffer]);
	glVertexAttribPointer(vColor, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, BUFFER_OFFSET(0));
#endif
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vColor);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
#ifdef __APPLE__
	glutInitDisplayMode(GLUT_RGBA | GLUT_3_2_CORE_PROFILE);
	glutCreateWindow(argv[0]);
#else
	glutInitDisplayMode(GLUT_RGBA);
	glutInitContextVersion(3,3);
	glutInitContextFlags (GLUT_CORE_PROFILE | GLUT_DEBUG);
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
