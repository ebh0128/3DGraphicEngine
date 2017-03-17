#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec2 texcoord2;
layout(location = 8) in mat4 World;


uniform mat4 M;
uniform mat4 V;
uniform mat4 VP;

out vec2 oTexcoord;
out vec3 oNormal;
out vec3 oViewPos;
out vec2 oTexCoordNoise;


void main()
{
	gl_Position = VP*World * M*Position;
	oTexcoord = texcoord;
	oTexCoordNoise = texcoord2;
	oNormal = normalize((V *World*M * vec4(Normal.xyz,0)).xyz);
	oViewPos = (V*World*M * Position).xyz;
	
}
