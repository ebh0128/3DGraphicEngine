#version 430 core
in vec4 vColor;


uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;

uniform vec2 gScreenSize;
uniform vec3 gEyeWorldPos;

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

vec2 CalcTexCoord()
{
	return gl_FragCoord.xy / gScreenSize;
}

out vec4 fColor;

void main()
{
	vec2 TexCoord = CalcTexCoord();
	vec3 WorldPos = texture(gPositionMap , TexCoord).xyz;
	vec3 Color = texture(gColorMap , TexCoord).xyz;
	vec3 Normal = texture(gNormalMap , TexCoord).xyz;
	Normal = normalize(Normal);
	
	vec4 LightResult = vec4(0,0,0,0);
	LightResult = CalcDirLight(gDirLight , WorldPos , Normal);
	
	//fColor = vec4(1,1,1,1);
	fColor = vec4(Color , 1.0)* LightResult;
	//fColor = vec4(Normal, 1.0) ;
	//fColor = LightResult; //*CalcPointLight(List[5] , WorldPos , Normal);
	//fColor = vec4(1,1,1,1)*(Count-8.9);
}

