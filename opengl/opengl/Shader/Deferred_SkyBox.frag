#version 330 core
in vec4 vColor;
in vec3 vTpos;



uniform samplerCube SkyBoxTexture;
uniform vec4 inversLightVector;
uniform vec4 DiffuseCol;

layout (location = 2) out vec3 DiffuseOut;
//Normal채널 보냄으로서 SkyBox와 DirLight 연산 정도를 조절

void main()
{
	DiffuseOut = (DiffuseCol*texture(SkyBoxTexture , vTpos)).xyz;
	
}

