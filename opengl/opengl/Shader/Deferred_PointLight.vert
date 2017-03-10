#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 5) in mat4 World;

uniform vec4 DiffuseCol;
uniform mat4x4 MVP;
uniform mat4	MV;
uniform mat4 M;
uniform mat4 VP;
flat out int InstanceID;

out vec4 vColor;
void main()
{
	gl_Position = VP*World*M*aPosition;
	
	
	vColor = vec4(DiffuseCol.xyz,1);
	InstanceID =  gl_InstanceID;
	
}
