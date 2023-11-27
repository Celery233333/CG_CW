#pragma once

#include "bitmap.h"

struct ShadowStruct
{
	unsigned int FBO;
	unsigned int Texture;
};

ShadowStruct setup_shadowmap(int w, int h)
{
	ShadowStruct shadow;
	glGenFramebuffers(1, &shadow.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
	glGenTextures(1, &shadow.Texture);
	glBindTexture(GL_TEXTURE_2D, shadow.Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,w,h,0,GL_DEPTH_COMPONENT, GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); 
	float borderColor[] = { 1.0f,  1.0f,  1.0f,  1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO); 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow.Texture, 0); 
	glDrawBuffer(GL_NONE); 
	glReadBuffer(GL_NONE); 
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return shadow;

}

void saveShadowMapToBitmap(unsigned int Texture, int w, int h)
{
	float* pixelBuffer = (float*)malloc(sizeof(float) * w * h);// [] ;
	glBindTexture(GL_TEXTURE_2D, Texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixelBuffer);

	char* charBuffer = (char*)malloc(sizeof(unsigned char) * w * h * 3);

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			charBuffer[(y * w * 3) + (x * 3) + 0] = pixelBuffer[(y * w) + x] * 255;
			charBuffer[(y * w * 3) + (x * 3) + 1] = pixelBuffer[(y * w) + x] * 255;
			charBuffer[(y * w * 3) + (x * 3) + 2] = pixelBuffer[(y * w) + x] * 255;
		}

	}

	BITMAPINFOHEADER infoHdr;
	infoHdr.biSize = 40;
	infoHdr.biWidth = w;
	infoHdr.biHeight = h;
	infoHdr.biPlanes = 1;
	infoHdr.biBitCount = 24;
	infoHdr.biCompression = 0;
	infoHdr.biSizeImage = sizeof(unsigned char) * w * h * 3;
	infoHdr.biXPelsPerMeter = 0;
	infoHdr.biYPelsPerMeter = 0;
	infoHdr.biClrUsed = 0;
	infoHdr.biClrImportant = 0;

	BITMAPFILEHEADER fileHdr;
	fileHdr.bfType = 19778;
	fileHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(unsigned char) * w * h * 3);
	fileHdr.bfReserved1 = 0;
	fileHdr.bfReserved2 = 0;
	fileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	savebitmap("shadowMap.bmp", (unsigned char*)charBuffer, &infoHdr, &fileHdr);

	free(charBuffer);
	free(pixelBuffer);
}