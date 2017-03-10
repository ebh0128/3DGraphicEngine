#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <random>
#include <cstdlib>
#include <memory.h>
#include <Windows.h>
#include <vector>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <GLUT/GLUT.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
//glm
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
//DevIL
#include <IL\il.h>
#include <IL/ilut.h>
//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#endif
#ifndef BUFFER_OFFSET	
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif

#define LIGHT_MAX 50
#define TEXCOORD_MAX 3


typedef struct TransformationSet
{
	glm::vec4 vPos;
	glm::vec3 vRot;
	glm::vec3 vScale;

}TransSet;

typedef struct LightAttnuation
{
	float Constant;
	float Linear;
	float exp;
}LitAttnu;