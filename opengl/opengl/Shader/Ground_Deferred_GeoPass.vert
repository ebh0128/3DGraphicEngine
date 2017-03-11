#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec2 texcoord2;

uniform mat4x4 WVP;
uniform mat4x4 MVP;
uniform mat4 World;
uniform mat4 MV;
uniform mat4 V;

out vec2 oTexcoord;
out vec3 oNormal;
out vec3 oViewPos;
out vec2 oTexCoordNoise;


void main()
{
	gl_Position = WVP * Position;
	oTexcoord = texcoord;
	oTexCoordNoise = texcoord2;
	oNormal = normalize((V *World * vec4(Normal.xyz,0)).xyz);
	oViewPos = (MV * Position).xyz;
	
}
