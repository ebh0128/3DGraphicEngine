#version 430 core
in vec4 vColor;


uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;
uniform sampler2D ShadowMap;
uniform sampler2D AOMap;


uniform vec2 gScreenSize;
uniform vec3 gEyeWorldPos;


uniform mat4 V;

uniform mat4 lightSpaceMat;
uniform mat4 InverseV;

//각 빛 성분의 4번째값은 factor
//DirLight 에서는 LPos 가 방향
struct LightInfo
{
	vec4 LPos;
	vec4 LDiff;
	vec4 LAmbi;
	vec4 LSpec;
	vec4 LAttnuation;
};

uniform LightInfo gDirLight;


vec2 CalcTexCoord()
{
	return gl_FragCoord.xy / gScreenSize;
}

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
			shadow += currentDepth -0.005 > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow /= 9.0;
	

	
	return shadow;
}


vec4 CalcLight(LightInfo Lit,
				vec3 LightDirection,
				vec3 ViewPos,
				vec3 Normal)
{
	vec4 AmbientCol =  Lit.LDiff*Lit.LAmbi * Lit.LAmbi.w;
	AmbientCol.w = 1;
	
	AmbientCol *= texture(AOMap , CalcTexCoord()).r;
	
	float DiffuseFactor = dot(Normal, -LightDirection);
	
	vec4 DiffuseColor = vec4(0,0,0,0);
	vec4 SpecularColor = vec4(0,0,0,0);
	
	if(DiffuseFactor > 0.0)
	{
		DiffuseColor = Lit.LDiff * DiffuseFactor * Lit.LDiff.w;
		DiffuseColor.w =1;
		
		vec3 VertexToEye = normalize(gEyeWorldPos - ViewPos);
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
	float shadow = ShadowCalculation( lightSpaceMat * InverseV * vec4(ViewPos,1) );
	
	return (AmbientCol + (1-shadow)*(DiffuseColor + SpecularColor));
//	return DiffuseColor;
}

vec4 CalcDirLight(LightInfo Lit,
					vec3 ViewPos,
					vec3 Normal)
{
	vec4 LightViewPos =V*vec4( Lit.LPos.xyz , 0) ;
	vec3 LightDir = normalize(LightViewPos.xyz);
	vec4 retColor = CalcLight(Lit , LightDir , ViewPos , Normal);
	return retColor;

}




out vec4 fColor;

void main()
{
	vec2 TexCoord = CalcTexCoord();
	vec3 ViewPos = texture(gPositionMap , TexCoord).xyz;
	vec3 Color = texture(gColorMap , TexCoord).xyz;
	vec3 Normal = texture(gNormalMap , TexCoord).xyz;
	Normal = normalize(Normal);
	
	vec4 LightResult = vec4(0,0,0,0);
	LightResult = CalcDirLight(gDirLight , ViewPos , Normal);
	
	//결과값
	fColor = vec4(Color , 1.0)* LightResult;
	
	
	//fColor =  LightResult;
	//fColor =  vec4(texture(ShadowMap , TexCoord).r);
	
	
}

