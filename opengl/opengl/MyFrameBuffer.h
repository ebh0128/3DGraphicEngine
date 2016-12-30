#pragma once

class MyFrameBuffers
{
protected:
	GLuint*		pTexColors;
	int ColorCount;

	GLuint		TexDepth;

	GLuint		fboID;
public:
	MyFrameBuffers();
	MyFrameBuffers(int ColorCnt, int Width, int Height);


};