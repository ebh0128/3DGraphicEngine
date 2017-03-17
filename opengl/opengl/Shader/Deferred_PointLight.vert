#version 430 core
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aNormal;
layout(location = 8) in mat4 World;
layout(location = 12) in mat4 LightAttnu;

uniform vec4 DiffuseCol;
uniform mat4x4 MVP;
uniform mat4	MV;
uniform mat4 M;
uniform mat4 VP;

uniform mat4 LightTransMat;

flat out int InstanceID;

flat out vec4 PtWorldPosition;
flat out vec4 PtDiffuse; 
flat out vec4 PtAmbient;
flat out vec4 PtLightAttnu;

out vec4 vColor;
void main()
{
	gl_Position = VP*World*M*aPosition;
	
	
	vColor = vec4(DiffuseCol.xyz,1);
	InstanceID =  gl_InstanceID;
	PtWorldPosition = World*M*vec4(0,0,0,1);
	PtDiffuse = LightAttnu[1];
	PtAmbient = LightAttnu[2];
	PtLightAttnu = LightAttnu[3];
	
}
