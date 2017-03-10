#version 430 core
out vec4 fColor;

in vec4 vPosEye;
in vec4 vNormal;
in vec2 vTexCoord;
in vec2 vTexCoordNoise;
in vec4 vWorldPosition;

uniform mat4 MV;
uniform mat4 V;

uniform vec3 gEyeWorldPos;

uniform sampler2D TextureMain;


struct sMaterial
{
	vec3	diffuse;
	vec3	ambient;
	vec3	specular;
	float 	shininess;
};

uniform sMaterial material;
uniform int IsTextured;

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
	//vec3
	vec4 LightColor = vec4(0.0);
	
	//Point
	for(int i = 0 ; i < Count ; i++)
	{
		LightColor += CalcPointLight(List[i] ,vWorldPosition.xyz , vNormal.xyz);
	}
	//Dir Light
	LightColor += CalcDirLight(gDirLight , vWorldPosition.xyz , vNormal.xyz);

	
	vec4 TexCol= vec4(0.6,0.6,0.6,1);
	if(IsTextured == 1)
	{
		TexCol= texture(TextureMain , vec2(vTexCoord.x , 1-vTexCoord.y));
	}
	
	fColor = LightColor*TexCol ;
	
	
}

