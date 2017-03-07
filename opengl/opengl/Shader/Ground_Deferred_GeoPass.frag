#version 330 core

in vec2 oTexcoord;
in vec3 oNormal;
in vec3 oWorldPos;
in vec2 oTexCoordNoise;

layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec3 DiffuseOut;
layout (location = 2) out vec3 NormalOut;
layout (location = 3) out vec3 TexCoordOut;


uniform sampler2D TextureGround;
uniform sampler2D TextureSnow;
uniform sampler2D TextureStone;
uniform sampler2D SamplerNoise;

void main()
{
	
	// Apply Texture by Noise(디퓨즈 계산)
	
	float noisecol = texture(SamplerNoise, oTexCoordNoise).r;
	noisecol = clamp(noisecol , 0 , 1);
	
	
	vec4 TexCol;
	vec4 SnowCol = texture(TextureSnow , oTexcoord);
	vec4 StoneCol = texture(TextureStone , oTexcoord);
	vec4 GroundCol = texture(TextureGround , oTexcoord);
	vec4 HightCol = noisecol * SnowCol + (1-noisecol)* StoneCol;
	vec4 LowCol = (1-noisecol)*GroundCol + noisecol*StoneCol;
	TexCol = noisecol*HightCol + (1-noisecol)* LowCol ;
	
	DiffuseOut = vec3(TexCol.r , TexCol.g , TexCol.b);
	//DiffuseOut = SnowCol.xyz;

	//DiffuseOut = vec3(oTexcoord , 0.0);
	
	WorldPosOut = oWorldPos;
	NormalOut = normalize(oNormal);
	TexCoordOut = vec3(oTexcoord , 0.0);
	
}