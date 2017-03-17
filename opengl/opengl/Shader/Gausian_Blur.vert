#version 430 core
layout(location = 0) in vec4 aPosition;

out vec2 TexCoord;
 
void main()
{
	gl_Position = aPosition;
	
	 //TexCoord 계산
	TexCoord = (aPosition.xy + vec2(1.0)) / 2.0;
}
