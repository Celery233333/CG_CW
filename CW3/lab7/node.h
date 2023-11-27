#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "objFile.h"

class Mesh;
class Material;
class Node
{
public:
			Node();
virtual	~Node();
	void	resetLocationMatrix();
	void	addChildNode(Node* node);
	void	setParent(Node* n) { m_Parent = n; }
	const glm::mat4& getWorldMatrix() { return m_WorldMatrix; }
	const glm::vec3& getPosition() { return m_Position; }
	virtual	void	resetWorldMatrix();
	void	createObj(ObjFile* objFile);
	void	createCube(Material* mtl, Texture* textures[5]);
	void	createSphere(Material* mtl, Texture* texture);
	void	createCylinder(Material* mtl, Texture* textures[2]);
	void	createCylinderRing(Material* mtl, Texture* textures[2]);
	void	setModel(Mesh* mesh, Material* mtl, Texture* texture);
	void	render(GLuint program);
	void	renderDepth(GLuint program);
	void	setPosition(glm::vec3 pos) { m_Position = pos; }
	void	setRotation(glm::vec3 r) { m_Rotation = r; }
	void	setScale(glm::vec3 s) { m_Scale = s; }
	void	rotateZ(float angle);

	void	mergeMesh(std::vector<float>& vertices, std::vector<int>& indices, const glm::mat4& parentMatrix);
	std::string meshString();
protected:
	Node* m_Parent;
	std::vector<Node*> m_Children;
	glm::vec3   m_Position;
	glm::vec3   m_Scale;
	glm::vec3   m_Rotation;
	glm::mat4	 m_LocationMatrix;
	glm::mat4	 m_WorldMatrix;

	Mesh* m_Mesh;
	Material* m_Material;
	Texture* m_Texture;
};

