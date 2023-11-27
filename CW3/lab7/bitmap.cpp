#include "bitmap.h"
GLuint loadbitmap(const char* filename, unsigned char*& pixelBuffer, BITMAPINFOHEADER* infoHeader, BITMAPFILEHEADER* fileHeader)
{
	FILE* bitmapFile;

	errno_t err = fopen_s(&bitmapFile, filename, "rb");
	if (err != 0 || bitmapFile == NULL)
	{
		printf("loadbitmap - open failed for %s\n", filename);
		return NULL;
	}

	fread(fileHeader, sizeof(BITMAPFILEHEADER), 1, bitmapFile);

	if (fileHeader->bfType != 0x4D42)
	{
		printf("loadbitmap - type failed \n");
		return NULL;
	}

	fread(infoHeader, sizeof(BITMAPINFOHEADER), 1, bitmapFile);

	if (infoHeader->biBitCount < 24)
	{
		printf("loadbitmap - bitcount failed = %d\n", infoHeader->biBitCount);
		return NULL;
	}

	fseek(bitmapFile, fileHeader->bfOffBits, SEEK_SET);

	int nBytes = infoHeader->biWidth * infoHeader->biHeight * 3;
	pixelBuffer = new unsigned char[nBytes];
	fread(pixelBuffer, sizeof(unsigned char), nBytes, bitmapFile);

	fclose(bitmapFile);

	for (int i = 0; i < nBytes; i += 3)
	{
		unsigned char tmp = pixelBuffer[i];
		pixelBuffer[i] = pixelBuffer[i + 2];
		pixelBuffer[i + 2] = tmp;
	}

	printf("loadbitmap - loaded %s w=%d h=%d bits=%d\n", filename, infoHeader->biWidth, infoHeader->biHeight, infoHeader->biBitCount);
}

GLuint savebitmap(const char* filename,
	unsigned char* pixelBuffer,
	BITMAPINFOHEADER* infoHeader, BITMAPFILEHEADER* fileHeader)
{
	FILE* bitmapFile;

	errno_t err = fopen_s(&bitmapFile, filename, "wb");
	if (err != 0 || bitmapFile == NULL)
	{
		printf("savebitmap - open failed for %s\n", filename);
		return NULL;
	}

	fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, bitmapFile);

	fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, bitmapFile);

	fseek(bitmapFile, fileHeader->bfOffBits, SEEK_SET);

	int nBytes = infoHeader->biWidth * infoHeader->biHeight * 3;
	fwrite(pixelBuffer, sizeof(unsigned char), nBytes, bitmapFile);

	fclose(bitmapFile);

	printf("savebitmap - saved %s w=%d h=%d bits=%d\n", filename, infoHeader->biWidth, infoHeader->biHeight, infoHeader->biBitCount);
}

