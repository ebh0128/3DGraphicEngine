#version 430 core
layout(location = 0) in vec4 aPosition;


uniform vec4 DiffuseCol;
uniform mat4x4 MVP;
uniform mat4	MV;
uniform mat4 M;
uniform mat4 VP;

out vec4 vColor;
void main()
{
	gl_Position = aPosition;
	
	
	vColor = vec4(DiffuseCol.xyz,1);
}
