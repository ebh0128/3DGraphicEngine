#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in vec3 WorldPos_GS_in[];
in vec2 TexCoord_GS_in[];
in vec3 Normal_GS_in[];


out vec3 WorldPos_FS_in;
out vec2 TexCoord_FS_in;
out vec3 Normal_FS_in;
out vec3 ViewPos_FS_in;

uniform mat4 VP;
uniform mat4 V;

void main()
{
	vec4 ViewPos1 = V*vec4(WorldPos_GS_in[0] , 1);
	vec4 ViewPos2 = V*vec4(WorldPos_GS_in[1], 1);
	vec4 ViewPos3 = V*vec4(WorldPos_GS_in[2], 1);
	//원래 해야되는 작업
	//0번 점
	gl_Position = VP*gl_in[0]. gl_Position;
	WorldPos_FS_in = WorldPos_GS_in[0];
	TexCoord_FS_in = TexCoord_GS_in[0];
	Normal_FS_in = Normal_GS_in[0];
	ViewPos_FS_in = ViewPos1.xyz;
	EmitVertex();
	
	gl_Position = VP*gl_in[1]. gl_Position;
	WorldPos_FS_in = WorldPos_GS_in[1];
	TexCoord_FS_in = TexCoord_GS_in[1];
	Normal_FS_in = Normal_GS_in[1];
	ViewPos_FS_in = ViewPos2.xyz;
	EmitVertex();
	
	gl_Position = VP*gl_in[2].gl_Position;
	WorldPos_FS_in = WorldPos_GS_in[2];
	TexCoord_FS_in = TexCoord_GS_in[2];
	Normal_FS_in = Normal_GS_in[2];
	ViewPos_FS_in = ViewPos3.xyz;
	EmitVertex();
	
	EndPrimitive();
	
	
}
