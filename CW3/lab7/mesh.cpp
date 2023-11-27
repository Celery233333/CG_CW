#include <vector>

#include "mesh.h"
Mesh* Mesh::s_SquareMesh = NULL;
Mesh* Mesh::s_CylinderMesh = NULL;
Mesh* Mesh::s_CylinderInsideMesh = NULL;
Mesh* Mesh::s_CircleMesh = NULL;
Mesh* Mesh::s_RingMesh = NULL;
Mesh* Mesh::s_SphereMesh = NULL;

void	Mesh::init()
{
	initSquare();
	initCylinder(0.5f, 1.0f,50,false);
	initCylinder(0.5f, 1.0f, 50, true);
	initCircle(0.5f,50);
	initRing(RING_INSIDE_RADIUS,0.5f, 50);
	initSphere(0.5f,50);
}
void	Mesh::unInit()
{
	delete s_SquareMesh;
	delete s_CylinderMesh;
	delete s_CylinderInsideMesh;
	delete s_RingMesh;
	delete s_CircleMesh;
	delete s_SphereMesh;
}

void	Mesh::initSquare()
{
	float vertices[] = {
		//pos					//Normal		// uv
		-0.5f, 0.0f, 0.5f,  	0.f, 1.f, 0.f,		0.f,0.f,
		0.5f, 0.0f, 0.5f,  		0.f, 1.f, 0.f,		1.f,0.f,
		0.5f, 0.0f, -0.5f,  	0.f, 1.f, 0.f,		1.f,1.f,
		-0.5f, 0.0f, -0.5f,  	0.f, 1.f, 0.f,		0.f,1.f,
	};

	int indices[] = {
		0,1,2,
		2,3,0,
	};

	s_SquareMesh = new Mesh();
	s_SquareMesh->create(vertices,4,indices,6);
}

void	Mesh::initRing(float insideRadius, float outsideRadius, int slices)
{
	std::vector<float> vertices;
	std::vector<int> indices;
	float thetaStep = (float)(M_PI * 2 / slices);
	float r[] = { insideRadius, outsideRadius };
	for (int i = 0; i < 2; i++) {
		float radius = r[i];
		float uvScale = radius / outsideRadius;
		for (int j = 0; j <= slices; ++j)
		{
			float theta = j * thetaStep;

			float x = cosf(theta);
			float z = sinf(theta);

			//pos
			vertices.push_back(x * radius);
			vertices.push_back(0.0f);
			vertices.push_back(z * radius);

			//normal
			vertices.push_back(0.0f);
			vertices.push_back(1.0f);
			vertices.push_back(0.0f);

			//uv
			vertices.push_back(x * uvScale * 0.5f  + 0.5f);
			vertices.push_back(0.5f - z * uvScale * 0.5f);

		}
	}
	for (int i = 0; i < slices; ++i) {
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + slices +1 +1);

		indices.push_back(i + slices + 1 + 1);
		indices.push_back(i + slices + 1);
		indices.push_back(i);
	}
	s_RingMesh = new Mesh();
	s_RingMesh->create(vertices.data(), (int)vertices.size()/8, indices.data(), (int)indices.size());
}
void	Mesh::initCircle(float radius, int slices)
{
	std::vector<float> vertices;
	std::vector<int> indices;
	float thetaStep = (float)(M_PI * 2 / slices);

	//pos
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);

	//normal
	vertices.push_back(0.0f);
	vertices.push_back(1.0f);
	vertices.push_back(0.0f);

	//uv
	vertices.push_back(0.5f);
	vertices.push_back(0.5f);


	for (int j = 0; j <= slices; ++j)
	{
		float theta = j * thetaStep;

		float x = cosf(theta);
		float z = sinf(theta);

		//pos
		vertices.push_back(x * radius);
		vertices.push_back(0.0f);
		vertices.push_back(z * radius);

		//normal
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);

		//uv
		vertices.push_back(x * 0.5f + 0.5f);
		vertices.push_back(0.5f - z * 0.5f);

	}

	for (int i = 0; i < slices; ++i) {
		indices.push_back(0);
		indices.push_back(i + 2);
		indices.push_back(i + 1);
	}
	s_CircleMesh = new Mesh();
	s_CircleMesh->create(vertices.data(), (int)vertices.size()/8, indices.data(), (int)indices.size());

}

void	Mesh::initCylinder(float radius, float height,int slices,bool isInside)
{
	std::vector<float> vertices;
	std::vector<int> indices;
	float pi2 = (float)(M_PI * 2);

	float thetaStep = pi2 / slices;

	for (int j = 0; j <= slices; ++j) {
		float theta = j * thetaStep;

		float x = cosf(theta);
		float z = sinf(theta);

		glm::vec3 n(x, 0, z);
		n = glm::normalize(n);
		if (isInside) {
			n = -n;
		}

		float y = 0.0f;
		float v = 0.0f;
		for (int i = 0; i < 2; i++) {
			//pos
			vertices.push_back(x * radius);
			vertices.push_back(y);
			vertices.push_back(z * radius);

			//normal
			vertices.push_back(n.x);
			vertices.push_back(n.y);
			vertices.push_back(n.z);

			//uv
			vertices.push_back(1.0f - theta / pi2);
			vertices.push_back(v);

			y += height;
			v += 1.0f;
		}
	}

	
	for (int i = 0; i < slices; ++i) {
		if (isInside) {
			indices.push_back((i + 1) * 2);
			indices.push_back(i * 2 + 1);
			indices.push_back(i * 2);

			indices.push_back(i * 2 + 1);
			indices.push_back((i + 1) * 2);
			indices.push_back((i + 1) * 2 + 1);
		}
		else {
			indices.push_back((i + 1) * 2);
			indices.push_back(i * 2);
			indices.push_back(i * 2 + 1);

			indices.push_back(i * 2 + 1);
			indices.push_back((i + 1) * 2 + 1);
			indices.push_back((i + 1) * 2);

		}
	

	}
	if (isInside) {
		s_CylinderInsideMesh = new Mesh();
		s_CylinderInsideMesh->create(vertices.data(), (int)vertices.size() /8, indices.data(), (int)indices.size());
	}
	else {
		s_CylinderMesh = new Mesh();
		s_CylinderMesh->create(vertices.data(), (int)vertices.size() / 8, indices.data(), (int)indices.size());

	}

}

void	Mesh::initSphere(float radius,int slices)
{

	std::vector<float> vertices;
	std::vector<int> indices;
	int stacks = slices / 2;
	float pi = (float)M_PI;
	float pi2 = (float)(M_PI*2);
	float phyStep = pi / stacks;
	float thetaStep = pi2 / slices;

	for (int i = 0; i <= stacks; ++i)
	{
		float phi = i * phyStep;

		for (int j = 0; j <= slices; ++j)
		{
			float theta = j * thetaStep;

			float x = cosf(theta) * sinf(phi);
			float y = cosf(phi);
			float z = sinf(theta) * sinf(phi);

			//pos
			vertices.push_back(x * radius);
			vertices.push_back(y * radius);
			vertices.push_back(z * radius);

			//normal
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			//uv
			vertices.push_back(1.0f - theta / pi2);
			vertices.push_back(phi / pi);

		}
	}

	for (int i = 0; i < slices * stacks + slices; ++i)
	{
		indices.push_back(i);
		indices.push_back(i + slices + 1);
		indices.push_back(i + slices);

		indices.push_back(i + slices + 1);
		indices.push_back(i);
		indices.push_back(i + 1);
	}
	s_SphereMesh = new Mesh();
	s_SphereMesh->create(vertices.data(), (int)vertices.size()/8, indices.data(), (int)indices.size());

}


Mesh::Mesh()
{
	m_VAO = 0;
	m_VBO = 0;
	m_EBO = 0;
	m_IndicesCount = 0;
	m_VerticesCount = 0;
	m_Vertices = NULL;
	m_Indices = NULL;
}
Mesh::~Mesh()
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

	if (m_Vertices) {
		delete[] m_Vertices;
		m_Vertices = NULL;
	}
	if (m_Indices) {
		delete [] m_Indices;
		m_Indices = NULL;
	}
}
void	Mesh::create(float* vertData, int verticesNum, int* indicesData, int IndicesNum)
{
	m_VerticesCount = verticesNum;
	m_IndicesCount = IndicesNum;

	m_Vertices = new float[m_VerticesCount * 8];
 	m_Indices = new int[m_IndicesCount];

	memcpy(m_Vertices, vertData, m_VerticesCount * 8 * sizeof(float));
	memcpy(m_Indices, indicesData, m_IndicesCount * sizeof(int));


	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesNum * 8, vertData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndicesNum * sizeof(int), indicesData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void	Mesh::draw()
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_INT, 0);
}
