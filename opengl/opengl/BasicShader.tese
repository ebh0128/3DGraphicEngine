#version 430 core

layout(triangles, equal_spacing, ccw) in;

in vec3 WorldPos_ES_in[];
in vec2 TexCoord_ES_in[];
in vec3 Normal_ES_in[];

uniform mat4 VP;

//out vec3 WorldPos_FS_in;
//out vec2 TexCoord_FS_in;
//out	vec3 Normal_FS_in;

out vec3 WorldPos_GS_in;
out vec2 TexCoord_GS_in;
out vec3 Normal_GS_in;

uniform sampler2D SamplerNoise;

uniform float gDispFactor;

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
	Normal_GS_in = interpolate3D(Normal_ES_in[0], Normal_ES_in[1], Normal_ES_in[2]);
	Normal_GS_in = normalize(Normal_GS_in);
	//좌표 재계산
	float Displacement  = texture(SamplerNoise , TexCoord_GS_in).x;
	WorldPos_GS_in += Normal_GS_in * Displacement * gDispFactor *0.1;                                
 
	
	gl_Position =  vec4(WorldPos_GS_in , 1.0);
	
}
