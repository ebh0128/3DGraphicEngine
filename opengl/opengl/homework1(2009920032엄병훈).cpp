///////////////////////////////////////////////////////////////////////
//
// 2009920032 엄병훈 과제 제출
// 2009920032 엄병훈 과제 제출
// 2009920032 엄병훈 과제 제출
// 2009920032 엄병훈 과제 제출
// 2009920032 엄병훈 과제 제출
// 2009920032 엄병훈 과제 제출
///////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
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
GLuint	vao;
GLuint	vbo[2], ebo;
GLuint	h_prog;
static GLuint	attrib_position = 2;
static GLuint	attrib_color = 7;
enum VBO_Bufs { VBO_POSITION = 0, VBO_COLOR, NUM_VBOS };

#include "shaders.cpp"

void init(void)
{
	GLfloat vertices[][3] = {
		{ -0.5, -0.5, 0.4 },
		{  0.9, -0.5, 0.4 },
		{ -0.5,  0.9, 0.4 },
		{  0.9,  0.9, 0.4 },
		{ -0.9, -0.9, 0.6 },
		{  0.5, -0.9, 0.6 },
		{ -0.9,  0.5, 0.6 },
		{  0.5,  0.5, 0.6 },
	};

	GLubyte colors[][4] = {
		{ 255, 255, 255, 192 },
		{ 0, 255, 255, 192 },
		{ 255,   0, 255, 192 },
		{ 255, 255,   0, 192 },
		{ 255,   0,   0, 255 },
		{ 0, 255,   0, 255 },
		{ 0,   0, 255, 255 },
		{ 0,   0,   0, 255 },
	};

	GLubyte	indices[][3] = {
		{ 4 ,5 ,6 },
		{ 6 ,5 ,7 },
		{ 0 ,1 ,2 },
		{ 2 ,1 ,3 },
	};

	h_prog = build_program_from_files("homework1.vert", "homework1.frag");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(NUM_VBOS, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(attrib_position,3,GL_FLOAT,GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrib_position);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_COLOR]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(attrib_color , 4 , GL_UNSIGNED_BYTE, GL_TRUE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrib_color);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	



	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	// Add as many OpenGL function calls as needed here...
}

void display(void)
{
	// You should NOT add any more function calls in display().
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(h_prog);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 3*4 , GL_UNSIGNED_BYTE , BUFFER_OFFSET(0));
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
#ifdef  __APPLE__
	glutInitDisplayMode(GLUT_RGBA | GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("homework #1");
#else
	glutInitDisplayMode(GLUT_RGBA| GLUT_DOUBLE |GLUT_DEPTH);
	glutInitContextVersion(3,3);
	glutInitContextFlags (GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitContextProfile(GLUT_FORWARD_COMPATIBLE);
	glutCreateWindow("homework #1");
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
#endif
	init();
	glutDisplayFunc(display);
	glutMainLoop();
}
