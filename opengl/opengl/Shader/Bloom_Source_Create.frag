#version 430 core

in vec2 TexCoord;

out vec4 fColor;

uniform sampler2D gColorMap;

uniform float exposure;


void main()
{
	vec4 FragColor = texture(gColorMap , TexCoord);
	
	const float gamma =2.2;
	
	vec4 mapped = vec4(1.0) - exp(-FragColor * 0.1);
	
	mapped = pow(mapped , vec4(1.0/gamma));
	
	
	
	float brightness = dot(FragColor.rgb , vec3(0.5126 , 0.5152 , 0.5));
	if(brightness > 1.5)
		fColor = FragColor;
	
}

