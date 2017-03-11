#version 430 core

in vec2 TexCoord;

out vec4 fColor;

uniform sampler2D gPositionMap;
uniform sampler2D gNormalMap;
uniform sampler2D gNoiseTexture;

uniform float gSampleRad;

uniform mat4 P;

const int MAX_KERNEL_SIZE = 64;

const vec2 noiseScale = vec2(1024.0/4.0 , 768.0/4.0);

uniform vec3 gKernel[MAX_KERNEL_SIZE];

void main()
{
	// 포지션 맵에서 뽑은 위치정보
	vec3 Pos = texture(gPositionMap , TexCoord).xyz;
	vec3 Normal = texture(gNormalMap , TexCoord).rgb;
	vec3 RandomVec = texture(gNoiseTexture , TexCoord).xyz;

	vec3 tangent = normalize(RandomVec - Normal* dot(RandomVec , Normal));
	vec3 bitangent = cross(Normal , tangent);
	mat3 TBN = mat3(tangent, bitangent, Normal);
		
	float AO = 0.0;
	
	for(int i = 0 ; i < MAX_KERNEL_SIZE ; i++)
	{
		vec3 Sample = TBN * gKernel[i];
		
		vec3 samplePos = Pos + gKernel[i] * gSampleRad;
		// samplePos = Pos + Sample * gSampleRad;
		// Sampling 한 지점 
		vec4 offset = vec4(samplePos , 1.0);
		offset = P * offset;
		
		//Clip space
		offset.xyz /= offset.w;
		
		// [-1 , 1] -> [0 , 1]
		offset.xyz = offset.xyz * 0.5 + vec3(0.5);
		
		//카메라 깊이갚 가져옴
		float sampleDepth = texture(gPositionMap , offset.xy).b;
		
		//경계선 안으로 들어간것
		//if(abs(Pos.z - sampleDepth) < gSampleRad)
		//{
			float rangeCheck = smoothstep(0.0 , 1.0 , gSampleRad / abs(Pos.z - sampleDepth));
			AO +=(sampleDepth >= samplePos.z +0.025 ? 1.0 :0.0)*rangeCheck;
		//}
	}
	
	AO = 1.0 - AO/64.0;
	fColor = vec4(pow(AO , 2.0));
}

