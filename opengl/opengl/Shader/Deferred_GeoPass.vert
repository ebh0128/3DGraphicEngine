#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec2 texCoord;
layout(location = 5) in mat4 World;


uniform mat4x4 MVP;
uniform mat4 MV;
uniform mat4 M;
uniform mat4 VP;
uniform mat4 V;


out vec2 oTexcoord;
out vec3 oNormal;
out vec3 oWorldPos;

void main()
{
	gl_Position = VP*World*M*Position;
	oTexcoord = texCoord;
	oNormal = normalize((World * M * vec4(Normal.xyz,0)).xyz);
	oWorldPos = (World * M *Position).xyz;
}
