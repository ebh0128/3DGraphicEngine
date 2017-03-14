#version 430 core
in vec4 vColor;
flat in int InstanceID;

flat in vec4 PtWorldPosition;
flat in vec4 PtDiffuse; 
flat in vec4 PtAmbient;
flat in vec4 PtLightAttnu;

struct sMaterial
{
	vec3	diffuse;
	vec3	ambient;
	vec3	specular;
	float 	shininess;
};

uniform sMaterial material;

uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;

uniform vec2 gScreenSize;
uniform vec3 gEyeWorldPos;

uniform mat4 V;
uniform mat4 M;
uniform mat4 LightTransMat;
//각 빛 성분의 4번째값은 factor
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

vec4 CalcLight(LightInfo Lit,
				vec3 LightDirection,
				vec3 ViewPos,
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
		
		vec3 VertexToEye = normalize( - ViewPos);
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
	//return  DiffuseColor ;
	
}

vec4 CalcPointLight(LightInfo Lit,
					vec3 ViewPos,
					vec3 Normal)
{
	vec4 LightViewPos =  V*Lit.LPos*M;
	//LightViewPos = V*PtWorldPosition*M;

	vec3 LightDirection = ViewPos - LightViewPos.xyz;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);
	
	vec4 retColor = CalcLight(Lit , LightDirection , ViewPos , Normal);
	
	//Attnuation Hard Coding
	//float Attnuation = 0.3 + Distance* 0.05 + Distance*Distance*0.05;
	
	float Attnuation = Lit.LAttnuation.x + Distance* Lit.LAttnuation.y + Distance*Distance*Lit.LAttnuation.z;
	
	
	Attnuation = max(1.0 , Attnuation);
	
	
	return retColor/ Attnuation;
	
}

vec2 CalcTexCoord()
{
	return gl_FragCoord.xy / gScreenSize;
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
	
	LightInfo InstanceLight;
	InstanceLight.LPos = PtWorldPosition;
	InstanceLight.LDiff = PtDiffuse;
	InstanceLight.LAmbi = PtAmbient;
	InstanceLight.LAttnuation = PtLightAttnu;
	InstanceLight.LSpec = vec4(0.5, 0.5, 0.5, 0.5);
	
	//LightResult = CalcPointLight(List[InstanceID % Count] , ViewPos , Normal);
	LightResult = CalcPointLight(InstanceLight , ViewPos , Normal);
	
	fColor = vec4(Color , 1.0) * LightResult;
	
	//fColor = vec4(Color , 1.0) * PtDiffuse ;
	//fColor = LightResult; //*CalcPointLight(List[5] , ViewPos , Normal);
	
}

