#version 430 core
out vec4 fColor;


uniform mat4 MV;
uniform mat4 V;
uniform vec3 gEyeWorldPos;

uniform sampler2D TextureDiffuse;
uniform sampler2D TextureNormal;
uniform sampler2D TextureDisplacement;



void main()
{
	
	//vec4 DifCol = texture(TextureDiffuse, TexCoord_FS_in);
	//vec4 NormalCol = texture(TextureNormal, TexCoord_FS_in);
	//vec4 DisplaceCol = texture(TextureDisplacement, TexCoord_FS_in);

	fColor = vec4(1,1,1,1);

	
	
}

