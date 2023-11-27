
#include "skybox.h"
Skybox::Skybox()
{
	m_VAO = 0;
	m_VBO = 0;
	m_EBO = 0;
	m_IndicesCount = 0;

}
Skybox::~Skybox()
{
	if (m_VAO) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
	if (m_VBO) {
		glDeleteBuffers(1, &m_VBO);
		m_VBO = 0;
	}
	if (m_EBO) {
		glDeleteBuffers(1, &m_EBO);
		m_EBO = 0;
	}
}

void	Skybox::create(float size)
{
	float vertices[] =
	{
		//front face
		-size, -size, size,
		-size,  size, size,
		 size,  size, size,
		 size, -size, size,

		//right face
		size, -size,  size,
		size,  size,  size,
		size,  size, -size,
		size, -size, -size,

		//back face
		 size, -size, -size,
		 size,  size, -size,
		-size,  size, -size,
		-size, -size, -size,

		//left face
		-size, -size, -size,
		-size,  size, -size,
		-size,  size,  size,
		-size, -size,  size,

		//top face
		-size, size,  size,
		 size, size,  size,
		 size, size, -size,
		-size, size, -size,

		//bottom face
		-size, -size,  size,
		 size, -size,  size,
		 size, -size, -size,
		-size, -size, -size,
	};
	int indices[] = {
		0, 2, 3, 2, 0, 1,
		4, 6, 7, 6, 4, 5,
		8, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 14, 15,
		16, 18, 17, 18, 16, 19,
		20, 21, 22, 20, 22, 23
	};

	int verticesNum = sizeof(vertices)/sizeof(float);
	m_IndicesCount = sizeof(indices) / sizeof(int);


	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

}

void	Skybox::render()
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_INT, 0);
}
