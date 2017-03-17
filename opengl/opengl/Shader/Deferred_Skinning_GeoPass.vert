#version 330 core
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec2 texCoord;
layout(location = 4) in ivec4 BoneIDs;
layout(location = 5) in vec4 Weight;
layout(location = 8) in mat4 World;


uniform mat4x4 MVP;
uniform mat4 MV;
uniform mat4 M;
uniform mat4 VP;
uniform mat4 V;

const int MAX_BONES = 100;

uniform mat4 gBones[MAX_BONES];
uniform mat4 gTest;

out vec2 oTexcoord;
out vec3 oNormal;
out vec3 oViewPos;


void main()
{
	mat4 BoneTransform 	 = gBones[BoneIDs[0]] * Weight[0];
	BoneTransform		+= gBones[BoneIDs[1]] * Weight[1];
	BoneTransform		+= gBones[BoneIDs[2]] * Weight[2];
	BoneTransform		+= gBones[BoneIDs[3]] * Weight[3];

	vec4 PosSkined = BoneTransform * vec4(Position.xyz , 1.0);
	
	gl_Position = VP*World*M*PosSkined;
	gl_Position = VP*World*M*Position;
	
	oTexcoord = texCoord;
	
	vec4 NormalSkinned = BoneTransform *vec4(Normal.xyz , 0.0);
	
	oNormal = normalize((V * World*M * vec4(NormalSkinned.xyz,0)).xyz);
	oNormal = normalize((V*World * M * vec4(Normal.xyz,0)).xyz);
	
	oViewPos = (V*World * M *Position).xyz;
	
}
