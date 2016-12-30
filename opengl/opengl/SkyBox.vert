#version 330 core
layout(location = 0) in vec4 aPosition;

uniform mat4	InversVP;

out vec3 vTpos;
void main()
{
	
	gl_Position = aPosition;
	vec4 front = InversVP * vec4(aPosition.xy , -1.0 , 1.0);
	vec4 back = InversVP * vec4(aPosition.xy , +1.0 , 1.0);
	vec3 Dir = back.xyz/ back.w - front.xyz / front.w;
	//Dir.y = 1-Dir.y;
	vTpos = Dir;
	
	
}
