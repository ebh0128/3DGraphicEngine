#define _USE_MATH_DEFINES
#include "CommonHeader.h"
#include "Geometry.h"


Sphere::Sphere(float Radius, unsigned int rings, unsigned int sectors)
{
	//링간의 변화량
	float const DeltaR = 1.f / (float)(rings - 1);
	//섹터간의 변화량
	float const DeltaS = 1.f / (float)(sectors - 1);
	
	int r, s;

	vertices.resize(rings * sectors * 3);
	normals.resize(rings * sectors * 3);
	texcoords.resize(rings * sectors * 2);

	std::vector<GLfloat>::iterator v = vertices.begin();
	std::vector<GLfloat>::iterator n = normals.begin();
	std::vector<GLfloat>::iterator t = texcoords.begin();

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			float const y = sin(-M_PI_2 + M_PI*r*DeltaR);
			float const x = cos(2 * M_PI * s * DeltaS) * sin(M_PI * r * DeltaR);
			float const z = sin(2*M_PI * s * DeltaS) * sin(M_PI * r * DeltaR);

			//텍스쳐 코드 st
			*t++ = s*DeltaS;
			*t++ = r*DeltaR;

			//버텍스
			*v++ = x*Radius;
			*v++ = y*Radius;
			*v++ = z*Radius;

			//노말
			*n++ = x;
			*n++ = y;
			*n++ = z;
		}

	}

	indices.resize(rings * sectors * 6);
	std::vector<GLuint>::iterator i = indices.begin();
	for (r = 0; r < rings - 1; r++)
	{
		for (s = 0; s < sectors - 1; s++)
		{
			*i++ = r * sectors + s;
			*i++ = (r + 1) * sectors + s;
			*i++ = (r + 1) * sectors + (s + 1);

			*i++ = r * sectors + s;
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = r * sectors + (s + 1);
		}
	}

}
Sphere::~Sphere()
{
	vertices.clear();
	normals.clear();
	texcoords.clear();
	indices.clear();
	std::vector<GLfloat>().swap(vertices);
	std::vector<GLfloat>().swap(normals);
	std::vector<GLfloat>().swap(texcoords);
	std::vector<GLuint>().swap(indices);

}