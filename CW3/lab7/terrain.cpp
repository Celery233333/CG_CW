#include <glm/gtc/noise.hpp>
#include "terrain.h"

Terrain::Terrain()
{

}
Terrain::~Terrain()
{
	if (m_Mesh) {
		delete m_Mesh;
		m_Mesh = NULL;
	}
}

void Terrain::create(float tileSize, int tileNum, float textureTileSize,float centerRadius)
{
	int x, z;
	int verticesNum = (tileNum + 1) * (tileNum + 1);
	int faceNum = tileNum * tileNum * 2;
	int indicesNum = faceNum * 3;
	int* indices = new int[indicesNum];
	glm::vec3* pPosition = new glm::vec3[verticesNum];
	glm::vec3* pNormal = new glm::vec3[verticesNum];
	glm::vec2* pUV = new glm::vec2[verticesNum];
	glm::vec3* pFaceNormal = new glm::vec3[faceNum];

	float totalSize = tileSize * tileNum;
	float startPosition = -totalSize * 0.5f;
	float maxRadius = sqrtf(totalSize * totalSize * 2);

	glm::vec3* p = pPosition;
	glm::vec2* uv = pUV;

	for (z = 0; z <= tileNum; z++) {
		for (x = 0; x <= tileNum; x++) {
			p->x = startPosition + tileSize * x;
			p->z = startPosition + tileSize * z;

			uv->x = p->x / textureTileSize;
			uv->y = -p->z / textureTileSize;

			p++;
			uv++;

		}
	}
	int index = 0;
	int v0, v1, v2, v3;
	for (z = 0; z < tileNum; z++) {
		for (x = 0; x < tileNum; x++) {

			v0 = (tileNum + 1) * z + x;
			v1 = v0 + 1;
			v2 = v0 + (tileNum + 1);
			v3 = v2 + 1;

			indices[index++] = v2;
			indices[index++] = v3;
			indices[index++] = v1;

			indices[index++] = v1;
			indices[index++] = v0;
			indices[index++] = v2;
		}
	}
	int i, j;

	float frequency[4] = {
		4.0f,8.0f,16.0f,32.0f
	};
	float amplitude[4] = {
		16.0f,8.0f,4.0f,2.0f
	};

	float minY = 0.0f;
	p = pPosition;
	for (i = 0; i < verticesNum; i++) {
		float posy = 0.0f;
		for (j = 0; j < 4; j++) {
			glm::vec2 pos(p->x / totalSize * frequency[j], p->z / totalSize * frequency[j]);
			posy += glm::perlin(pos) * amplitude[j]*5;
		}
		if (posy < minY) {
			minY = posy;
		}
		p->y = posy;
		p++;
	}
	p = pPosition;
	for (i = 0; i < verticesNum; i++) {
		p->y -= minY;

		float radius = sqrtf(p->x * p->x + p->z * p->z);
		if (radius < centerRadius) {
			p->y = 0.0f;
		}
		else {
			p->y *= (radius - centerRadius) / maxRadius;
		}

		p++;
	}

	for (i = 0; i < faceNum; i++) {
		index = i * 3;
		v0 = indices[index];
		v1 = indices[index + 1];
		v2 = indices[index + 2];
		glm::vec3& p1 = pPosition[v0];
		glm::vec3& p2 = pPosition[v1];
		glm::vec3& p3 = pPosition[v2];

		pFaceNormal[i] = glm::normalize(glm::cross((p2 - p1), (p3 - p1)));

	}
	int f[6];
	glm::vec3* n = pNormal;
	for (z = 0; z <= tileNum; z++) {
		for (x = 0; x <= tileNum; x++) {
			f[0] = tileNum * (z-1) * 2 + x * 2;
			f[1] = f[0] + 1;
			f[2] = f[0] - 2;

			f[3] = f[0] + tileNum * 2 - 1;
			f[4] = f[3] - 1;
			f[5] = f[3] + 2;

			if (x == 0) {
				f[2] = -1;
				f[3] = -1;
				f[4] = -1;
			}
			if (x == tileNum) {
				f[0] = -1;
				f[1] = -1;
				f[5] = -1;
			}

			if (z == 0) {
				f[0] = -1;
				f[1] = -1;
				f[2] = -1;
			}
			if (z == tileNum) {
				f[3] = -1;
				f[4] = -1;
				f[5] = -1;
			}

			glm::vec3 pn(0, 0, 0);
			for (i = 0; i < 6; i++) {
				if (f[i] < 0) continue;
				pn += pFaceNormal[f[i]];
			}
			*n = glm::normalize(pn);

			n++;
		}
	}

	float* vertices = new float[verticesNum*8];
	p = pPosition;
	uv = pUV;
	n = pNormal;
	index = 0;
	for (i = 0; i < verticesNum; i++) {
		vertices[index++] = p->x;
		vertices[index++] = p->y;
		vertices[index++] = p->z;

		vertices[index++] = n->x;
		vertices[index++] = n->y;
		vertices[index++] = n->z;

		vertices[index++] = uv->x;
		vertices[index++] = uv->y;

		p++;
		n++;
		uv++;
	}
	Mesh* mesh = new Mesh();
	mesh->create(vertices, verticesNum, indices, indicesNum);
	delete [] vertices;
	delete[] indices;
	delete[] pPosition;
	delete[] pNormal;
	delete[] pUV;
	delete[] pFaceNormal;

	Material* mtl = new Material();
	setModel(mesh, mtl, Texture::getTexture(TEXTURE_ID_TERRAIN));

}