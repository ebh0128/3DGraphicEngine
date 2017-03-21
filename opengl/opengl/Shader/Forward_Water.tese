#version 430 core

layout(triangles, equal_spacing, ccw) in;

in vec3 WorldPos_ES_in[];
in vec2 TexCoord_ES_in[];
in vec3 Normal_ES_in[];
in vec3 Tangent_ES_in[];

uniform mat4 VP;

//out vec3 WorldPos_FS_in;
//out vec2 TexCoord_FS_in;
//out	vec3 Normal_FS_in;

out vec3 WorldPos_GS_in;
out vec2 TexCoord_GS_in;
out vec3 Normal_GS_in;
out vec3 Tangent_GS_in;
out vec2 New_TexCoord_GS_in;

uniform sampler2D TextureNormal;
uniform sampler2D TextureDisplacement;

uniform float gDispFactor;
uniform float TileS;
uniform float TileT;

uniform float gDeltaS;
uniform float gDeltaT;

vec2 interpolate2D(vec2 v0 , vec2 v1 , vec2 v2)
{
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) *v1 + vec2(gl_TessCoord.z) *v2; 
}

vec3 interpolate3D(vec3 v0 , vec3 v1 , vec3 v2)
{
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) *v1 + vec3(gl_TessCoord.z) *v2; 
}

void main()
{
	TexCoord_GS_in = interpolate2D(TexCoord_ES_in[0] , TexCoord_ES_in[1] , TexCoord_ES_in[2]);
	WorldPos_GS_in = interpolate3D(WorldPos_ES_in[0] , WorldPos_ES_in[1] , WorldPos_ES_in[2]);
	
	Tangent_GS_in  = interpolate3D(Tangent_ES_in[0]  , Tangent_ES_in[1] , Tangent_ES_in[2]);
	Tangent_GS_in = normalize(Tangent_GS_in);
	
	Normal_GS_in = interpolate3D(Normal_ES_in[0], Normal_ES_in[1], Normal_ES_in[2]);
	Normal_GS_in = normalize(Normal_GS_in);
	
	//탄젠트 정규화
	Tangent_GS_in = normalize(Tangent_GS_in - dot (Tangent_GS_in , Normal_GS_in) * Normal_GS_in);
	
	vec3 biTangent = cross(Tangent_GS_in , Normal_GS_in);
	
	//TBN 행렬 만듦
	mat3 TBN = mat3(Tangent_GS_in , biTangent , Normal_GS_in);
	
	//노말 가져와야됨
	Normal_GS_in = texture(TextureNormal , TexCoord_GS_in).xyz;
	// 색상 범위를 0~1 -> 노말 범위 -1~1
	// 원래 노말 완성
	Normal_GS_in = 2.0*Normal_GS_in - vec3(1.0 , 1.0 , 1.0);
	Normal_GS_in = TBN *Normal_GS_in;
	Normal_GS_in = normalize(Normal_GS_in);
	
	float Displacement  = texture(TextureDisplacement , TexCoord_GS_in).x;

	
	// 델타값으로 텍스쳐 코드 조절
	New_TexCoord_GS_in = vec2(TexCoord_GS_in.x + gDeltaS , TexCoord_GS_in.y + gDeltaT); 

	// 조절된 텍스쳐코드로 다시 노말 계산
	vec3 NewNormal = texture(TextureNormal , New_TexCoord_GS_in).xyz;
	NewNormal = 2*NewNormal - vec3 (1 , 1 ,1);
	NewNormal = TBN *NewNormal;
	NewNormal = normalize(NewNormal);
	
	//다시 디플 계산
	float NewDisplacement  = texture(TextureDisplacement , New_TexCoord_GS_in).x;

	//합치기
	Normal_GS_in = Normal_GS_in+ NewNormal;
	Normal_GS_in = normalize(Normal_GS_in);
	
	Displacement  = (Displacement + NewDisplacement);
	
	//좌표 재계산
	WorldPos_GS_in += vec3(0,1,0) * Displacement * gDispFactor ;                                
 
	
	gl_Position =  vec4(WorldPos_GS_in , 1.0);
	
}
