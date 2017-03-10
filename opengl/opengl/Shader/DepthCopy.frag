#version 430 core

uniform sampler2D gDepthTexture;

uniform vec2 gScreenSize;

vec2 CalcTexCoord()
{
	return gl_FragCoord.xy / gScreenSize;
}

void main()
{
	gl_FragDepth = texture(gDepthTexture , CalcTexCoord()).r;
}

