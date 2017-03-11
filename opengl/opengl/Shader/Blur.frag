#version 430 core

in vec2 TexCoord;

out vec4 fColor;

uniform sampler2D gColorMap;

float Offset[4] = float[](-1.5 , -0.5 , 0.5 , 1.5);

void main()
{
	vec3 Color = vec3(0.0 , 0.0 ,0.0);
	
	for(int i = 0 ; i < 4 ; i++)
	{
		for(int j = 0 ; j < 4 ; j++)
		{
			vec2 tc = TexCoord;
			tc.x = TexCoord.x + Offset[j] / textureSize(gColorMap , 0).x;
			tc.y = TexCoord.y + Offset[i] / textureSize(gColorMap , 0).y;
			Color += texture(gColorMap ,tc).xyz;
		}
	}
	
	Color /= 16.0;
	
	fColor = vec4(Color , 1.0);
}

