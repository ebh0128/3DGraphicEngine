#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec2 texcoord2;
layout(location = 5) in mat4 World;

uniform mat4 M;
uniform mat4 V;
uniform mat4 VP;

//View Dimension Position
out vec4 vPosEye;
out vec4 vNormal;
out vec2 vTexCoord;
out vec2 vTexCoordNoise;
out vec4 vPosWorld;

void main()
{
	gl_Position = VP*World * M*aPosition;
	vNormal = normalize(M*vec4(aNormal.xyz,0));
	vPosEye = V*World*M*aPosition;
	vTexCoord = texcoord;
	vTexCoordNoise = texcoord2;
	vPosWorld = World*M*aPosition;
}
