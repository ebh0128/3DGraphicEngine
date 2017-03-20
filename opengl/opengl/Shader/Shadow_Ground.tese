#version 430 core

layout(triangles, equal_spacing, ccw) in;

in vec3 WorldPos_ES_in[];


out vec3 WorldPos_GS_in;




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
	WorldPos_GS_in = interpolate3D(WorldPos_ES_in[0] , WorldPos_ES_in[1] , WorldPos_ES_in[2]);
 
	gl_Position =  vec4(WorldPos_GS_in , 1.0);
	
}
