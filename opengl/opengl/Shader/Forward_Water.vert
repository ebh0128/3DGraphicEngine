#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 texcoord;
layout(location = 6) in vec4 Tangent;

layout(location = 8) in mat4 World;

uniform mat4 M;
uniform mat4 V;
uniform mat4 VP;


out vec3 WorldPos_CS_in;
out vec2 TexCoord_CS_in;
out vec3 Normal_CS_in;
out vec3 Tangent_CS_in;

void main()
{
	WorldPos_CS_in = (World * M*aPosition).xyz;
	Normal_CS_in = normalize(World*M*vec4(aNormal.xyz,0)).xyz;
	TexCoord_CS_in = texcoord;
	Tangent_CS_in = normalize(World*M*vec4(Tangent.xyz,0)).xyz;
}
