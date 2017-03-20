#version 430 core

in vec3 WorldPos_FS_in;
in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;
in vec3 ViewPos_FS_in;

layout (location = 0) out vec3 TexCoordOut;
layout (location = 1) out vec3 ViewPosOut;
layout (location = 2) out vec3 DiffuseOut;
layout (location = 3) out vec3 NormalOut;


uniform sampler2D TextureGround;
uniform sampler2D TextureSnow;
uniform sampler2D TextureStone;
uniform sampler2D SamplerNoise;

uniform float TileS;
uniform float TileT;

void main()
{
	
 
	
	// Apply Texture by Noise(디퓨즈 계산)
	
	float noisecol = texture(SamplerNoise, TexCoord_FS_in).r;
	noisecol = clamp(noisecol , 0 , 1);
	
	vec2 NewTexCoord = vec2(TexCoord_FS_in.x * TileS, TexCoord_FS_in.y * TileT);
	
	vec4 TexCol;
	vec4 SnowCol = texture(TextureSnow , NewTexCoord);
	vec4 StoneCol = texture(TextureStone , NewTexCoord);
	vec4 GroundCol = texture(TextureGround , NewTexCoord);
	vec4 HightCol = noisecol * SnowCol + (1-noisecol)* StoneCol;
	vec4 LowCol = (1-noisecol)*GroundCol + noisecol*StoneCol;
	TexCol = noisecol*HightCol + (1-noisecol)* LowCol ;
	
	DiffuseOut = vec3(TexCol.r , TexCol.g , TexCol.b);

	ViewPosOut = ViewPos_FS_in;
	NormalOut = normalize(Normal_FS_in);
	//TexCoordOut = vec3(oTexcoord , 0.0);
	TexCoordOut = vec3(gl_FragCoord.zzz);
	
}