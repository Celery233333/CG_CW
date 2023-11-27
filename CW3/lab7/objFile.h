#pragma once
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "material.h"
#include "texture.h"

struct ObjMesh
{
	std::string mtlName;
	Mesh* mesh;
};

struct ObjMaterial
{
	Material* mtl;
	Texture* tex;
};

struct ObjModel {
	Mesh* mesh;
	Material* mtl;
	Texture* tex;
};

class ObjFile
{
public:
	ObjFile();
	~ObjFile();
	bool	load(const char* fileName);
	const std::vector<ObjModel*>& getModels() { return m_Models; }
private:
	bool	loadFace(std::istringstream& lineStream, std::vector<glm::ivec3>& indices);
	bool	loadMaterial(const std::string& path, const std::string& mtlLibName, std::map<std::string, ObjMaterial*>& mtls);
	void createMesh(const std::string& mtlName,
		const std::vector<glm::vec3>& vs,
		const std::vector<glm::vec3>& vts,
		const std::vector<glm::vec3>& vns,
		const std::vector<glm::ivec3>& indices,
		std::vector<ObjMesh*>& meshes);

private:
	std::map<std::string, Texture*> m_Textures;
	std::vector<ObjModel*> m_Models;

};

