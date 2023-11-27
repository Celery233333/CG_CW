#include <string>
#include "texture.h"
#include "bitmap.h"

Texture* Texture::s_Textures[TEXTURE_ID_COUNT];

Texture* Texture::getTexture(int id)
{
	if (id < 0 || id >= TEXTURE_ID_COUNT) return NULL;
	return s_Textures[id];
}

void Texture::init()
{
	for (int i = 0; i < TEXTURE_ID_COUNT; i++) {
		s_Textures[i] = new Texture();
	}
	s_Textures[TEXTURE_ID_TERRAIN]->load("res/terrain.bmp");
	s_Textures[TEXTURE_ID_CAMOUFLAGE]->load("res/camouflage.bmp");
	s_Textures[TEXTURE_ID_JEEP_SIDE_WINDOW]->load("res/jeep_side_window.bmp");
	s_Textures[TEXTURE_ID_JEEP_FRONT_WINDOW]->load("res/jeep_front_window.bmp");
	s_Textures[TEXTURE_ID_LIGHT]->load("res/light.bmp");
	s_Textures[TEXTURE_ID_Q]->load("res/q.bmp");
	s_Textures[TEXTURE_ID_SKYBOX]->loadCube("res/skybox");

}

void Texture::unInit()
{
	for (int i = 0; i < TEXTURE_ID_COUNT; i++) {
		delete s_Textures[i];
	}
}

Texture::Texture()
{
	m_TextureID = 0;
}
Texture::~Texture()
{
	if (m_TextureID) {
		glDeleteTextures(1, &m_TextureID);
		m_TextureID = 0;
	}
}
bool Texture::load(const char* fileName)
{
	unsigned char* pxls = NULL;
	BITMAPINFOHEADER info;
	BITMAPFILEHEADER file;
	loadbitmap(fileName, pxls, &info, &file);

	if (!pxls) return false;

	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info.biWidth, info.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pxls);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] pxls;

	return true;
}

bool Texture::loadCube(const char* path)
{
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	const char* fileName[] = {
		"right.bmp",
		"left.bmp",
		"top.bmp",
		"bottom.bmp",
		"front.bmp",
		"back.bmp",
	};

	for (int i = 0; i < 6;  i++) {
		unsigned char* pxls = NULL;
		BITMAPINFOHEADER info;
		BITMAPFILEHEADER file;
		std::string filename = path;
		filename += "/";
		filename += fileName[i];
		loadbitmap(filename.c_str(), pxls, &info, &file);
		if (!pxls) return false;
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, info.biWidth, info.biHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pxls);

		delete[] pxls;
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}