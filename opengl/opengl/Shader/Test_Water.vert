#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec2 texcoord;

layout(location = 8) in mat4 World;

uniform mat4 M;
uniform mat4 V;
uniform mat4 VP;


void main()
{
	gl_Position =  VP*(World * M*aPosition);

}
