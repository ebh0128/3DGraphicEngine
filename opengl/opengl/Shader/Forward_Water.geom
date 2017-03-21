#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in vec3 WorldPos_GS_in[];
in vec2 TexCoord_GS_in[];
in vec3 Normal_GS_in[];
in vec2 New_TexCoord_GS_in[];
in vec3 Tangent_GS_in[];

out vec3 WorldPos_FS_in;
out vec2 TexCoord_FS_in;
out vec2 New_TexCoord_FS_in;
out vec3 Normal_FS_in;
out vec3 Tangent_FS_in;

uniform mat4 VP;


void main()
{
	
	//원래 해야되는 작업
	//0번 점
	gl_Position = VP*gl_in[0]. gl_Position;
	WorldPos_FS_in = WorldPos_GS_in[0];
	TexCoord_FS_in = TexCoord_GS_in[0];
	New_TexCoord_FS_in = New_TexCoord_GS_in[0];
	Normal_FS_in = Normal_GS_in[0];
	Tangent_FS_in = Tangent_GS_in[0];
	EmitVertex();
	
	gl_Position = VP*gl_in[1]. gl_Position;
	WorldPos_FS_in = WorldPos_GS_in[1];
	TexCoord_FS_in = TexCoord_GS_in[1];
	New_TexCoord_FS_in = New_TexCoord_GS_in[1];
	Normal_FS_in = Normal_GS_in[1];
	Tangent_FS_in = Tangent_GS_in[1];
	EmitVertex();
	
	gl_Position = VP*gl_in[2].gl_Position;
	WorldPos_FS_in = WorldPos_GS_in[2];
	TexCoord_FS_in = TexCoord_GS_in[2];
	New_TexCoord_FS_in = New_TexCoord_GS_in[2];
	Normal_FS_in = Normal_GS_in[2];
	Tangent_FS_in = Tangent_GS_in[2];
	EmitVertex();
	
	EndPrimitive();
	
	
}
