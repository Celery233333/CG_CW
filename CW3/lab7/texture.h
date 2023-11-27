#pragma once
#include <GL/gl3w.h>
enum {
	TEXTURE_ID_TERRAIN,
	TEXTURE_ID_CAMOUFLAGE,
	TEXTURE_ID_JEEP_SIDE_WINDOW,
	TEXTURE_ID_JEEP_FRONT_WINDOW,
	TEXTURE_ID_LIGHT,
	TEXTURE_ID_Q,
	TEXTURE_ID_SKYBOX,
	TEXTURE_ID_COUNT,
};
class Texture
{
public:
	static void	init();
	static void	unInit();
	static Texture* getTexture(int id);

	Texture();
	~Texture();
	bool	load(const char* fileName);
	bool	loadCube(const char* path);
	GLuint getTextureID() { return m_TextureID; }
private:
	static	Texture* s_Textures[TEXTURE_ID_COUNT];
	GLuint m_TextureID;
};

