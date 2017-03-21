#version 430 core
out vec4 fColor;

in vec3 WorldPos_FS_in;
in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;
in vec2 New_TexCoord_FS_in;
in vec3 Tangent_FS_in;


uniform mat4 MV;
uniform mat4 V;
uniform vec3 gEyeWorldPos;

uniform sampler2D TextureDiffuse;
uniform sampler2D TextureNormal;
uniform sampler2D TextureDisplacement;
uniform sampler2D ShadowMap;


uniform mat4 lightSpaceMat;

uniform float TileS;
uniform float TileT;

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


float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(ShadowMap , projCoords.xy).r;
	float currentDepth = projCoords.z;

	if(projCoords.z > 1.0) return 0;
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(ShadowMap , 0);
	
	for(int x = -1 ; x <= 1 ; ++x)
	{
		for(int y = -1 ; y <= 1 ; ++y)
		{
			float pcfDepth = texture(ShadowMap, projCoords.xy + vec2(x , y) * texelSize).r;
			shadow += currentDepth -0.009 > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow /= 9.0;
	

	
	return shadow;
}


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
			SpecularFactor = pow(SpecularFactor , 10);
			SpecularColor =  Lit.LDiff * Lit.LSpec * SpecularFactor;
			SpecularColor.w = 1;
		}
	}
	float shadow = ShadowCalculation( lightSpaceMat * vec4(WorldPos, 1) );
	
	
	return (AmbientCol + (1-shadow)*(DiffuseColor + SpecularColor));
//	return (1-shadow)*vec4(1,1,1,1);

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
	
	vec3 BaseNormal = vec3(0,1,0);
	//탄젠트 정규화
	vec3 vTangent = normalize(Tangent_FS_in - dot (Tangent_FS_in , BaseNormal) * BaseNormal);
	
	vec3 biTangent = cross(vTangent , BaseNormal);
	
	//TBN 행렬 만듦
	mat3 TBN = mat3(vTangent , biTangent , BaseNormal);
	

	//합성 전 원래 위치 노말
	vec3 Normal = texture(TextureNormal , TexCoord_FS_in).xyz;
	Normal = 2*Normal - vec3(1,1,1);
	Normal = TBN*Normal;
	Normal = normalize(Normal);
	
	//합성할 노말
	vec3 NewNormal = texture(TextureNormal , New_TexCoord_FS_in).xyz;
	NewNormal = 2*NewNormal - vec3(1,1,1);
	NewNormal = TBN*NewNormal;
	NewNormal = normalize(NewNormal);
	
	Normal = normalize(Normal + NewNormal); 
	
	//Point
	for(int i = 0 ; i < Count ; i++)
	{
		LightColor += CalcPointLight(List[i] ,WorldPos_FS_in , Normal);
	}
	//Dir Light
	LightColor += CalcDirLight(gDirLight , WorldPos_FS_in , Normal);
	
	vec4 DifCol = (texture(TextureDiffuse, New_TexCoord_FS_in) + texture(TextureDiffuse, New_TexCoord_FS_in) )/2;
	
	vec4 NormalCol = texture(TextureNormal, TexCoord_FS_in);
	vec4 DisplaceCol = vec4(texture(TextureDisplacement, TexCoord_FS_in).r);

	fColor = LightColor* DifCol;
	fColor.a = 0.8;
	//fColor = LightColor ;
	
	//fColor = vec4(Normal.xyz ,1);	

	
	
}

