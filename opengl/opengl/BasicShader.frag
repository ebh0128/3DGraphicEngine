#version 430 core
out vec4 fColor;

in vec4 vPosEye;
in vec4 vNormal;
in vec2 vTexCoord;
in vec2 vTexCoordNoise;
in vec4 vPosWorld;

uniform mat4 MV;
uniform mat4 V;
uniform vec3 gEyeWorldPos;

uniform sampler2D TextureGround;
uniform sampler2D TextureSnow;
uniform sampler2D TextureStone;
uniform sampler2D SamplerNoise;

struct sMaterial
{
	vec3	diffuse;
	vec3	ambient;
	vec3	specular;
	float 	shininess;
};

uniform sMaterial material;

struct LightInfo
{
	vec4 LPos;
	vec4 LDiff;
	vec4 LAmbi;
	vec4 LSpec;
	vec4 LAttnuation;
};

layout( std140, binding = 1) uniform LightInfoList
{
	int Count;
	LightInfo List[256];
};


uniform LightInfo gDirLight;

vec4 CalcLight(LightInfo Lit,
				vec3 LightDirection,
				vec3 WorldPos,
				vec3 Normal)
{
	vec4 AmbientCol = Lit.LDiff * Lit.LAmbi * Lit.LAmbi.w;
	AmbientCol.w = 1;
	
	float DiffuseFactor = dot(Normal, -LightDirection);
	
	vec4 DiffuseColor = vec4(0,0,0,0);
	vec4 SpecularColor = vec4(0,0,0,0);
	
	if(DiffuseFactor > 0.0)
	{
		DiffuseColor = Lit.LDiff * DiffuseFactor * Lit.LDiff.w;
		DiffuseColor.w =1;
		
		vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos);
		vec3 LightReflect = normalize(reflect(LightDirection , Normal));
		float SpecularFactor = dot(VertexToEye , LightReflect);
		
		if(SpecularFactor > 0.0)
		{
			// Hard Coding
			SpecularFactor = pow(SpecularFactor , 1);
			SpecularColor = Lit.LDiff * Lit.LSpec * SpecularFactor;
			SpecularColor.w = 1;
		}
	}
	
	return (AmbientCol + DiffuseColor + SpecularColor);
}

vec4 CalcDirLight(LightInfo Lit,
					vec3 WorldPos,
					vec3 Normal)
{
	vec3 LightDir = normalize(Lit.LPos.xyz);
	vec4 retColor = CalcLight(Lit , LightDir , WorldPos , Normal);
	return retColor;

}
vec4 CalcPointLight(LightInfo Lit,
					vec3 WorldPos,
					vec3 Normal)
{
	vec3 LightDirection = WorldPos - Lit.LPos.xyz;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);
	
	vec4 retColor = CalcLight(Lit , LightDirection , WorldPos , Normal);
	
	//Attnuation Hard Coding
	//float Attnuation = 0.3 + Distance* 0.05 + Distance*Distance*0.05;
	
	float Attnuation = Lit.LAttnuation.x + Distance* Lit.LAttnuation.y + Distance*Distance*Lit.LAttnuation.z;
	
	
	Attnuation = max(1.0 , Attnuation);
	
	
	return retColor/ Attnuation;

}



void main()
{
	vec4 LightColor = vec4(0.0);
	
	//Point
	for(int i = 0 ; i < Count ; i++)
	{
		LightColor += CalcPointLight(List[i] ,vPosWorld.xyz , vNormal.xyz);
	}
	//Dir Light
	LightColor += CalcDirLight(gDirLight , vPosWorld.xyz , vNormal.xyz);
	
	// Apply Texture by Noise
	float noisecol = texture(SamplerNoise, vTexCoordNoise).r;
	noisecol = clamp(noisecol , 0 , 1);
	
	
	vec4 TexCol;
	vec4 SnowCol = texture(TextureSnow , vTexCoord);
	vec4 StoneCol = texture(TextureStone , vTexCoord);
	vec4 GroundCol = texture(TextureGround , vTexCoord);
	vec4 HightCol = noisecol * SnowCol + (1-noisecol)* StoneCol;
	vec4 LowCol = (1-noisecol)*GroundCol + noisecol*StoneCol;
	TexCol = noisecol*HightCol + (1-noisecol)* LowCol ;
	
	
	fColor = LightColor * TexCol;

	
	//fColor = List[0].LDiff;
	//fColor = GetSpotLightColor();
	//fColor =  List[1].LPos/10 ;
	//fColor =  n ;
	
	//fColor = TexCol ;
	
	
}

