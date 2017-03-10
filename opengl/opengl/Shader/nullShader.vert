#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 5) in mat4 World;


uniform mat4 M;
uniform mat4 VP;

out vec4 vColor;
void main()
{
	gl_Position = VP*World*M*aPosition;
	
}