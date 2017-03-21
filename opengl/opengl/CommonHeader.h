#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <cerrno>
#include <random>
#include <cstdlib>
#include <memory.h>
#include <Windows.h>
#include <vector>
#include <list>
#include <map>
#include <sstream>
#include <math.h>
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
#include <glm/gtx/quaternion.hpp>
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
#define KERNEL_SIZE 64
#define TEXCOORD_MAX 3

#define POSITION_LOCATION		0
#define NORMAL_LOCATION			1
#define TEX_COORD_LOCATION		2
#define TEX_COORD2_LOCATION		3
#define BONE_ID_LOCATION		4
#define BONE_WEIGHT_LOCATION	5
#define TANGENT_LOCATION		6
#define INSTANCE_LOCATION		8




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

typedef struct Vector
{
	GLfloat x; GLfloat y; GLfloat z;
}Vec;
typedef struct IndexVector
{
	GLuint A; GLuint B; GLuint C;
}InVec;

typedef struct TexCoord
{
	GLfloat x; GLfloat y;
}Tex;


#include "Util.h"
#include "MyShader.h"
#include "Texture.h"
#include "Mesh.h"
#include "ObjectInstance.h"
#include "Object.h"

