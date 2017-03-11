#version 430 core

in vec2 TexCoord;

out vec4 fColor;

uniform sampler2D gFinalMap;

uniform float exposure;


void main()
{
	// 포지션 맵에서 뽑은 위치정보
	const float gamma =2.2;
	vec4 HDRColor = texture(gFinalMap , TexCoord);

	vec4 mapped = vec4(1.0) - exp(-HDRColor * exposure);
	
	mapped = pow(mapped , vec4(1.0/gamma));
	
	fColor = mapped;
}

