#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include <windows.h>
#include <wingdi.h>
#include <GL/gl3w.h>

GLuint loadbitmap(const char* filename, unsigned char*& pixelBuffer, BITMAPINFOHEADER* infoHeader, BITMAPFILEHEADER* fileHeader);
GLuint savebitmap(const char* filename,unsigned char* pixelBuffer,BITMAPINFOHEADER* infoHeader, BITMAPFILEHEADER* fileHeader);


#endif //BITMAP_H