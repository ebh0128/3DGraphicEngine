#version 330 core

in vec2 oTexcoord;
in vec3 oNormal;
in vec3 oWorldPos;

layout (location = 0) out vec3 TexCoordOut;
layout (location = 1) out vec3 WorldPosOut;
layout (location = 2) out vec3 DiffuseOut;
layout (location = 3) out vec3 NormalOut;

uniform sampler2D TextureMain;
uniform int IsTextured;

void main()
{
	WorldPosOut = oWorldPos;
	if(IsTextured == 1) DiffuseOut = texture(TextureMain , oTexcoord).xyz;
	else DiffuseOut = vec3(1,1,1);
	NormalOut = normalize(oNormal);
	//TexCoordOut = vec3(oTexcoord , 0.0);
	TexCoordOut = vec3(gl_FragCoord.zzz);
}