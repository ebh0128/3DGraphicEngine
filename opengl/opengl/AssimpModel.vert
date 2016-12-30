#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 texcoord;
layout(location = 5) in mat4 World;

uniform mat4x4 MVP;
uniform mat4 MV;
uniform mat4 M;
uniform mat4 VP;
uniform mat4 V;

//View Dimension Position
out vec4 vPosEye;
out vec4 vNormal;
out vec2 vTexCoord;
out vec2 vTexCoordNoise;
out vec4 vWorldPosition;
flat out int InstanceID;

void main()
{
	gl_Position = VP*World*M*aPosition;
	vNormal = normalize(MV*vec4(aNormal.xyz,0));
	vPosEye = V*World*M*aPosition;
	vTexCoord = texcoord;
	InstanceID = gl_InstanceID;
	vWorldPosition = World*M*aPosition;
}
