#version 430 core
out vec4 fColor;

in vec4 vPosEye;
in vec4 vNormal;
in vec2 vTexCoord;
in vec2 vTexCoordNoise;
in vec4 vPosWorld;

uniform mat4 MV;
uniform mat4 V;

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
};

layout( std140, binding = 1) uniform LightInfoList
{
	int Count;
	LightInfo List[100];
};

struct SpotLight
{
	vec4 Diffuse;
	vec4 vPosition;
	vec4 vDirection;
	float fConeCosine;
	float fLinearAttnuation;
};

uniform SpotLight sLight;

vec4 GetSpotLightColor()
{
	//위치 벡터
	vec3 vDir = (vPosWorld - sLight.vPosition).xyz;
	float fDistance = length(vDir);
	vDir = normalize(vDir);
	
	//위치와 빛 방향 내적
	float fCosine = dot(sLight.vDirection.xyz , vDir);
	float fDif = 1.0 - sLight.fConeCosine;
	float fFactor = clamp((fCosine - sLight.fConeCosine)/ fDif , 0.0 , 1.0);
	
	if(fCosine > sLight.fConeCosine) return sLight.Diffuse*fFactor/(fDistance*sLight.fLinearAttnuation);
	return vec4(0,0,0,0);
}

void main()
{
	//vec3
	vec3 n = normalize(vNormal.xyz);
	vec3 l;
	vec3 v = normalize(-vPosEye.xyz);
	vec4 LightColor = vec4(0.0);
		
for(int i = 0 ; i < Count ; i++)
	{
		
	
		vec3 diffuse = vec3(0.0);
		vec3 specular = vec3(0.0);
		vec3 ambient = vec3(0.0);

		
		
		//point light?
		if(List[i].LPos.w == 1.0)
		{
			l = normalize((V*List[i].LPos - vPosEye).xyz);
			
			//half vector
			vec3 h = normalize(l+v);
			float l_dot_n = max(dot(l,n) , 0.0);
			
			//point Light Attnuation
			float dist = length((V*List[i].LPos - vPosEye).xyz);
			
			if(dist < 20)
			{
				float attnu = 1/ (0.3+0.05f*dist + 0.05f*pow(dist, 2));
				
				diffuse = List[i].LDiff.rgb * material.diffuse * l_dot_n;
				diffuse = diffuse*attnu;
				ambient = List[i].LAmbi.rgb * material.ambient;
				
				if(l_dot_n > 0.0)
				{
					specular = List[i].LSpec.xyz *material.specular * pow(max(dot(h,n) , 0.0), material.shininess);
				}
			}
			
		}
		
		else//directional light
		{
			l = normalize((V*List[i].LPos).xyz);
			//half vector
			vec3 h = normalize(l+v);
			float l_dot_n = max(dot(l,n) , 0.0);
			
			diffuse = List[i].LDiff.rgb * material.diffuse * l_dot_n;
			//diffuse = vec3(1.0 ,0 ,0);
			ambient = List[i].LAmbi.rgb * material.ambient;
			
			if(l_dot_n > 0.0)
			{
				specular = List[i].LSpec.xyz *material.specular * pow(max(dot(h,n) , 0.0), material.shininess);
			}
		
		}
		
		LightColor += vec4(ambient + diffuse + specular,1);
	}
	
	//SpotLight
	LightColor += GetSpotLightColor();
	
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

