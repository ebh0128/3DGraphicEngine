#version 330 core
in vec4 vColor;
in vec3 vTpos;
out vec4 fColor;

uniform samplerCube SkyBoxTexture;
uniform vec4 DiffuseCol;

void main()
{
	fColor = DiffuseCol*texture(SkyBoxTexture , vTpos);

}

