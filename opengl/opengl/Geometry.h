#pragma once

class Sphere
{
public:
	std::vector<GLfloat>	vertices;
	std::vector<GLfloat>	normals;
	std::vector<GLfloat>	texcoords;
	std::vector<GLuint>		indices;

public:
	Sphere(float Radius, unsigned int rings, unsigned int sectors);
	~Sphere();
};