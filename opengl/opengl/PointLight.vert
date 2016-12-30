#version 330 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;

uniform vec4 DiffuseCol;
uniform mat4x4 MVP;
uniform mat4	MV;

out vec4 vColor;
void main()
{
	gl_Position = MVP*aPosition;
	
	
	vColor = vec4(DiffuseCol.xyz,1);
}
