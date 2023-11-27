#include <iostream>
#include <fstream>
#include <sstream>
#include "objFile.h"

ObjFile::ObjFile()
{

}
ObjFile::~ObjFile()
{
	for (ObjModel* objModel : m_Models) {
		delete objModel->mesh;
		objModel->mtl->release();
		delete objModel;
	}
	std::map<std::string, Texture*>::iterator it = m_Textures.begin();
	std::map<std::string, Texture*>::iterator itEnd = m_Textures.end();
	while (it != itEnd) {
		Texture* tex = it->second;
		delete tex;
		it++;
	}

}
bool	ObjFile::loadFace(std::istringstream& lineStream, std::vector<glm::ivec3>& indices)
{
	std::vector<glm::ivec3> face;
	std::string vert;
	std::istringstream vertStream;
	std::string element;
	int i;
	face.clear();
	while (1) {
		vert = "";
		lineStream >> vert;
		if (vert.empty()) break;
		vertStream.clear();
		vertStream.str(vert);
		i = 0;
		glm::ivec3 v(0);
		while (i < 3 && std::getline(vertStream, element, '/')) {
			v[i++] = atoi(element.c_str());
		}
		if (i != 3) {
			std::cerr << "Error: obj face need v/vt/vn" << std::endl;
			return false;
		}
		face.push_back(v);
	}
	size_t n = face.size();
	if (n < 3) {
		return false;
	}
	for (i = 1; i < n-1; i++) {
		indices.push_back(face[0]);
		indices.push_back(face[i]);
		indices.push_back(face[i + 1]);
	}
	return true;
}
bool	ObjFile::loadMaterial(const std::string& path, const std::string& mtlLibName, std::map<std::string, ObjMaterial*>& mtls)
{
	std::string filePath = path + mtlLibName;
	std::ifstream input(filePath);
	if (!input.is_open()) {
		std::cerr << "Error: could not open file %s: " << filePath << std::endl;
		return false;
	}
	std::string line,flag;
	std::istringstream lineStream;
	ObjMaterial* mtl = NULL;
	;
	while (std::getline(input, line)) {
		lineStream.clear();
		lineStream.str(line);
		flag = "";
		lineStream >> flag;
		if (flag == "newmtl") {
			if (mtl && mtl->tex) {
				mtl->mtl->m_Kd = glm::vec3(1);
			}
			std::string mtlName;
			mtl = new ObjMaterial();
			mtl->mtl = new Material();
			mtl->tex = NULL;
			lineStream >> mtlName;
			mtls[mtlName] = mtl;
		}
		else if (mtl) {
			if (flag == "Ka") {
				lineStream >> mtl->mtl->m_Ka.x >> mtl->mtl->m_Ka.y >> mtl->mtl->m_Ka.z;
			}
			else if (flag == "Kd") {
				lineStream >> mtl->mtl->m_Kd.x >> mtl->mtl->m_Kd.y >> mtl->mtl->m_Kd.z;
			}
			else if (flag == "Ks") {
				lineStream >> mtl->mtl->m_Ks.x >> mtl->mtl->m_Ks.y >> mtl->mtl->m_Ks.z;
			}
			else if (flag == "Ns") {
				lineStream >> mtl->mtl->m_Ns;
			}
			else if (flag == "map_Kd") {
				std::string texName;
				lineStream >> texName;
				Texture* tex = m_Textures[texName];
				if (!tex) {
					tex = new Texture();
					std::string texPath = path + texName;
					tex->load(texPath.c_str());
					m_Textures[texName] = tex;
				}
				mtl->tex = tex;
			}
		}
	}
	input.close();
	if (mtl && mtl->tex) {
		mtl->mtl->m_Kd = glm::vec3(1);
	}

	return true;
}
void ObjFile::createMesh(const std::string& mtlName, 
	const std::vector<glm::vec3>& vs, 
	const std::vector<glm::vec3>& vts, 
	const std::vector<glm::vec3>& vns, 
	const std::vector<glm::ivec3>& indices, 
	std::vector<ObjMesh*>& meshes)
{
	if (indices.empty()) return;

	std::vector<int> meshIndices;
	std::vector<glm::ivec3> verts;
	size_t i,j, vNum;
	size_t num = indices.size();
	for (i = 0; i < num; i++) {
		const glm::ivec3& v = indices[i];

		vNum = verts.size();
		for (j = 0; j < vNum; j++) {
			if (verts[j] == v) {
				break;
			}
		}
		if (j == vNum) {
			verts.push_back(v);
		}
		meshIndices.push_back((int)j);
	}
	vNum = verts.size();
	float* vertData = new float[vNum * 8];
	i = 0;
	for (j = 0; j < vNum; j++) {
		const glm::ivec3& vi = verts[j];
		const glm::vec3& v = vs[vi.x-1];
		const glm::vec3& vt = vts[vi.y-1];
		const glm::vec3& vn = vns[vi.z-1];
		vertData[i++] = v.x;
		vertData[i++] = v.y;
		vertData[i++] = v.z;

		vertData[i++] = vn.x;
		vertData[i++] = vn.y;
		vertData[i++] = vn.z;

		vertData[i++] = vt.x;
		vertData[i++] = vt.y;
	}
	int* indicesData = meshIndices.data();
	Mesh* mesh = new Mesh();
	mesh->create(vertData,(int)vNum, indicesData, (int)meshIndices.size());
	delete [] vertData;

	ObjMesh* objMesh = new ObjMesh();
	objMesh->mtlName = mtlName;
	objMesh->mesh = mesh;
	meshes.push_back(objMesh);
}

bool	ObjFile::load(const char* fileName)
{
	std::ifstream input(fileName);
	if (!input.is_open()) {
		std::cerr << "Error: could not open file %s: " << fileName << std::endl;
		return NULL;
	}
	std::string path = fileName;
	size_t pos = path.find_last_of('/');
	

	if (pos != std::string::npos) {
		path = path.substr(0, pos+1);
	}

	std::string line, flag;
	std::vector<glm::vec3> vs, vts, vns;
	glm::vec3 v,vt,vn;
	std::string mtlName;
	std::istringstream lineStream;
	std::vector<glm::ivec3> indices;
	std::map<std::string, ObjMaterial*> mtls;
	std::vector<ObjMesh*> meshes;
	while (std::getline(input, line)) {
		lineStream.clear();
		lineStream.str(line);
		flag = "";
		lineStream >> flag;
		if (flag == "mtllib") {
			std::string mtlLibName;
			lineStream >> mtlLibName;
			loadMaterial(path,mtlLibName, mtls);
		}
		else if (flag == "v") {
			lineStream >> v.x >> v.y >> v.z;
			vs.push_back(v);
		}
		else if (flag == "vt") {
			lineStream >> vt.x >> vt.y;
			vts.push_back(vt);
		}
		else if (flag == "vn") {
			lineStream >> vn.x >> vn.y >> vn.z;
			vns.push_back(vn);
		}
		else if (flag == "f") {
			loadFace(lineStream, indices);
		}
		else if (flag == "usemtl") {
			createMesh(mtlName, vs, vts, vns, indices, meshes);
			lineStream >> mtlName;
			indices.clear();
		}
	}
	input.close();
	createMesh(mtlName, vs, vts, vns, indices, meshes);

	for (ObjMesh* objMesh : meshes) {
		ObjModel* objModel = new ObjModel();
		objModel->mesh = objMesh->mesh;
		ObjMaterial* objMtl = mtls[objMesh->mtlName];
		if (objMtl) {
			objModel->mtl = objMtl->mtl;
			objModel->tex = objMtl->tex;
			objModel->mtl->addRef();
		}
		else {
			objModel->mtl = new Material();
			objModel->tex = NULL;
		}
		m_Models.push_back(objModel);

		delete objMesh;
	}

	std::map<std::string, ObjMaterial*>::iterator it = mtls.begin();
	std::map<std::string, ObjMaterial*>::iterator itEnd = mtls.end();
	while(it != itEnd) {
		ObjMaterial* objMtl = it->second;

		objMtl->mtl->release();
		delete objMtl;
		it++;
	}

	return true;
}