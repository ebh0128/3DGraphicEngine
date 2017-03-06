#version 330 core

in vec2 oTexcoord;
in vec3 oNormal;
in vec3 oWorldPos;

layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec3 DiffuseOut;
layout (location = 2) out vec3 NormalOut;
layout (location = 3) out vec3 TexCoordOut;


uniform sampler2D TextureMain;

void main()
{
	WorldPosOut = oWorldPos;
	DiffuseOut = texture(TextureMain , oTexcoord).xyz;
	NormalOut = normalize(oNormal);
	TexCoordOut = vec3(oTexcoord , 0.0);
}