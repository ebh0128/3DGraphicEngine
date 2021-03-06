#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec2 texcoord2;
layout(location = 8) in mat4 World;

uniform mat4 M;
uniform mat4 lightSpaceMat;

out vec3 WorldPos_CS_in;

void main()
{
	WorldPos_CS_in = (World *M * Position).xyz;
	
}
