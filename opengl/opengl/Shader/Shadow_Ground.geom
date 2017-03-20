#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in vec3 WorldPos_GS_in[];

uniform mat4 lightSpaceMat;
void main()
{
	//원래 해야되는 작업
	//0번 점
	gl_Position = lightSpaceMat*gl_in[0]. gl_Position;
	EmitVertex();
	
	gl_Position = lightSpaceMat*gl_in[1]. gl_Position;
	EmitVertex();
	
	gl_Position = lightSpaceMat*gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
	
	
}
