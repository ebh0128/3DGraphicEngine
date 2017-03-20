#version 430 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec2 texcoord;

layout(location = 8) in mat4 World;


uniform mat4 M;
uniform mat4 V;
uniform mat4 VP;

out vec3 WorldPos_CS_in;
out vec2 TexCoord_CS_in;
out vec3 Normal_CS_in;



void main()
{
	//oViewPos = (V*World*M * Position).xyz;
	
	WorldPos_CS_in = (World * M*Position).xyz;
	Normal_CS_in = normalize(World*M*vec4(Normal.xyz,0)).xyz;
	TexCoord_CS_in = texcoord;
}
