#version 430 core


layout (vertices =3) out;

uniform vec3 gEyeWorldPos;   

uniform float gTessLevel;

in vec3 WorldPos_CS_in[];
in vec2 TexCoord_CS_in[];
in vec3 Normal_CS_in[];

out vec3 WorldPos_ES_in[];
out vec2 TexCoord_ES_in[];
out vec3 Normal_ES_in[];



void main()
{
	TexCoord_ES_in[gl_InvocationID] = TexCoord_CS_in[gl_InvocationID];
	Normal_ES_in[gl_InvocationID] = Normal_CS_in[gl_InvocationID];
	WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];
	
	// 레벨 지정
	gl_TessLevelOuter[0] = gTessLevel;
	gl_TessLevelOuter[1] = gTessLevel;
	gl_TessLevelOuter[2] = gTessLevel;
	gl_TessLevelInner[0] = gTessLevel;
	

}
