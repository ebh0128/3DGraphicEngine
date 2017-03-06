#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec2 texcoord2;

uniform mat4x4 MVP;
uniform mat4 MV;

//View Dimension Position
out vec4 vPosEye;
out vec4 vNormal;
out vec2 vTexCoord;
out vec2 vTexCoordNoise;
out vec4 vPosWorld;

void main()
{
	gl_Position = MVP*aPosition;
	vNormal = normalize(MV*vec4(aNormal.xyz,0));
	vPosEye = MV*aPosition;
	vTexCoord = texcoord;
	vTexCoordNoise = texcoord2;
	vPosWorld = aPosition;
}
