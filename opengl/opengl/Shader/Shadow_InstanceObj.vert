#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec2 texCoord;
layout(location = 8) in mat4 World;


uniform mat4 M;
uniform mat4 lightSpaceMat;


void main()
{
	gl_Position = lightSpaceMat*World*M*Position;
	
	
}
