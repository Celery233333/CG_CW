#pragma once
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <string>

#define RING_INSIDE_RADIUS 0.3f
class Mesh
{
public:
	static void	init();
	static void	unInit();
	static void	initSquare();
	static void	initRing(float insideRadius, float outsideRadius, int slices);
	static void	initCylinder(float radius, float height, int slices, bool isInside);
	static void	initCircle(float radius, int slices);
	static void	initSphere(float radius, int slices);
	static  Mesh* s_SquareMesh;
	static  Mesh* s_CylinderMesh;
	static  Mesh* s_CylinderInsideMesh;
	static  Mesh* s_CircleMesh;
	static  Mesh* s_RingMesh;
	static  Mesh* s_SphereMesh;

	Mesh();
	~Mesh();
	void	create(float* vertData, int verticesNum, int* indicesData, int IndicesNum);
	void	draw();

	int m_IndicesCount;
	int m_VerticesCount;
	float* m_Vertices;
	int* m_Indices;

private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;


};

