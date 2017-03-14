#version 330 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 5) in mat4 World;
layout(location = 9) in mat4 LightAttnu;

uniform vec4 DiffuseCol;
uniform mat4x4 MVP;
uniform mat4	MV;
uniform mat4 M;
uniform mat4 VP;

out vec4 vColor;
flat out vec4 PtDiffuse; 
void main()
{
	gl_Position = VP*World*M*aPosition;
	
	
	vColor = vec4(DiffuseCol.xyz,1);
	PtDiffuse = LightAttnu[1];
}
