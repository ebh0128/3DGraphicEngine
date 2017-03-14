#version 330 core
in vec4 vColor;
out vec4 fColor;

flat in vec4 PtDiffuse; 

struct sMaterial
{
	vec3	diffuse;
	vec3	ambient;
	vec3	specular;
	float 	shininess;
};

uniform sMaterial material;

void main()
{
	fColor = PtDiffuse *vColor;
}

