#pragma once
#include <GL/gl3w.h>
class Skybox
{
public:
	Skybox();
	~Skybox();
	void	create(float size);
	void	render();
private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;
	int m_IndicesCount;
};

