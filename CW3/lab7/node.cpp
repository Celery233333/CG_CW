#include <sstream>
#include "node.h"

Node::Node()
{
	m_Position = glm::vec3(0);
	m_Scale = glm::vec3(1);
	m_Rotation = glm::vec3(0);
	m_LocationMatrix = glm::mat4(1);
	m_WorldMatrix = glm::mat4(1);
	m_Parent = NULL;
	m_Mesh = NULL;
	m_Material = NULL;
	m_Texture = NULL;
}

Node::~Node()
{
	setModel(NULL, NULL, NULL);

	for (Node* n : m_Children) {
		delete n;
	}
	m_Children.clear();
	m_Parent = NULL;
}

void	Node::resetLocationMatrix()
{
	glm::mat4 matIdentity(1);
	glm::mat4 matS = glm::scale(matIdentity, m_Scale);
	glm::mat4 matT = glm::translate(matIdentity, m_Position);
	glm::mat4 rotX = glm::rotate(matIdentity, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
	glm::mat4 rotY = glm::rotate(matIdentity, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
	glm::mat4 rotZ = glm::rotate(matIdentity, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
	m_LocationMatrix = matT * rotZ * rotY * rotX * matS;
	resetWorldMatrix();
}

void	Node::resetWorldMatrix()
{
	if (m_Parent) {
		m_WorldMatrix = m_Parent->getWorldMatrix() * m_LocationMatrix;
	}
	else {
		m_WorldMatrix = m_LocationMatrix;
	}

	for (Node* n : m_Children) {
		n->resetWorldMatrix();
	}
}

void	Node::addChildNode(Node* node)
{
	node->setParent(this);
	m_Children.push_back(node);
}

void	Node::setModel(Mesh* mesh, Material* mtl, Texture* texture)
{
	m_Mesh = mesh;
	m_Texture = texture;
	if (m_Material) {
		m_Material->release();
	}
	m_Material = mtl;
	if (m_Material) {
		m_Material->addRef();
	}

}

void	Node::createObj(ObjFile* objFile)
{
	const std::vector<ObjModel*>& models = objFile->getModels();
	for (ObjModel* model : models) {
		Node* node = new Node();
		node->setModel(model->mesh, model->mtl, model->tex);
		addChildNode(node);
	}
}

void	Node::createCube(Material* mtl, Texture* textures[5])
{
	//top
	Node* node = new Node();
	node->setModel(Mesh::s_SquareMesh, mtl, textures[0]);
	node->setPosition(glm::vec3(0,1,0));
	node->resetLocationMatrix();
	addChildNode(node);

	//bottom
	node = new Node();
	node->setModel(Mesh::s_SquareMesh, mtl, textures[0]);
	node->setRotation(glm::vec3(0, 0, 180));
	node->resetLocationMatrix();
	addChildNode(node);

	//front
	node = new Node();
	node->setModel(Mesh::s_SquareMesh, mtl, textures[1]);
	node->setRotation(glm::vec3(90, 0, 0));
	node->setPosition(glm::vec3(0, 0.5, 0.5));
	node->resetLocationMatrix();
	addChildNode(node);

	//back
	node = new Node();
	node->setModel(Mesh::s_SquareMesh, mtl, textures[2]);
	node->setRotation(glm::vec3(90, 180, 0));
	node->setPosition(glm::vec3(0, 0.5, -0.5));
	node->resetLocationMatrix();
	addChildNode(node);

	//left
	node = new Node();
	node->setModel(Mesh::s_SquareMesh, mtl, textures[3]);
	node->setRotation(glm::vec3(90, -90, 0));
	node->setPosition(glm::vec3(-0.5, 0.5, 0));
	node->resetLocationMatrix();
	addChildNode(node);

	//right
	node = new Node();
	node->setModel(Mesh::s_SquareMesh, mtl, textures[4]);
	node->setRotation(glm::vec3(90, 90, 0));
	node->setPosition(glm::vec3(0.5, 0.5, 0));
	node->resetLocationMatrix();
	addChildNode(node);

}
void	Node::createSphere(Material* mtl, Texture* texture)
{
	setModel(Mesh::s_SphereMesh, mtl, texture);
}
void	Node::createCylinder(Material* mtl, Texture* textures[2])
{
	//top
	Node* node = new Node();
	node->setModel(Mesh::s_CircleMesh, mtl, textures[0]);
	node->setPosition(glm::vec3(0, 1, 0));
	node->resetLocationMatrix();
	addChildNode(node);

	//bottom
	node = new Node();
	node->setModel(Mesh::s_CircleMesh, mtl, textures[0]);
	node->setRotation(glm::vec3(180, 0, 0));
	node->resetLocationMatrix();
	addChildNode(node);

	//side
	node = new Node();
	node->setModel(Mesh::s_CylinderMesh, mtl, textures[1]);
	addChildNode(node);

}

void	Node::createCylinderRing(Material* mtl, Texture* textures[2])
{
	//top
	Node* node = new Node();
	node->setModel(Mesh::s_RingMesh, mtl, textures[0]);
	node->setPosition(glm::vec3(0, 1, 0));
	node->resetLocationMatrix();
	addChildNode(node);

	//bottom
	node = new Node();
	node->setModel(Mesh::s_RingMesh, mtl, textures[0]);
	node->setRotation(glm::vec3(180, 0, 0));
	node->resetLocationMatrix();
	addChildNode(node);

	//outside
	node = new Node();
	node->setModel(Mesh::s_CylinderMesh, mtl, textures[1]);
	node->resetLocationMatrix();
	addChildNode(node);

	//inside
	node = new Node();
	node->setModel(Mesh::s_CylinderInsideMesh, mtl, textures[1]);
	float s = RING_INSIDE_RADIUS / 0.5f;
	node->setScale(glm::vec3(s, 1, s));
	node->resetLocationMatrix();
	addChildNode(node);

}

void Node::render(GLuint program)
{

	if (m_Mesh) {
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(m_WorldMatrix));

		glUniform3fv(glGetUniformLocation(program, "Ka"), 1, glm::value_ptr(m_Material->m_Ka));
		glUniform3fv(glGetUniformLocation(program, "Kd"), 1, glm::value_ptr(m_Material->m_Kd));
		glUniform3fv(glGetUniformLocation(program, "Ks"), 1, glm::value_ptr(m_Material->m_Ks));
		glUniform1f(glGetUniformLocation(program, "Ns"), m_Material->m_Ns);


		if (m_Texture) {
			glBindTexture(GL_TEXTURE_2D, m_Texture->getTextureID());
		}

		m_Mesh->draw();
	}

	for (Node* n : m_Children) {
		n->render(program);
	}
}

void	Node::renderDepth(GLuint program)
{

	if (m_Mesh) {
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(m_WorldMatrix));
		m_Mesh->draw();
	}

	for (Node* n : m_Children) {
		n->renderDepth(program);
	}
}

void	Node::rotateZ(float angle)
{
	angle += m_Rotation.z;
	if (angle > 360.0f) {
		angle -= 360.0f;
	}
	else if (angle < 0.0f) {
		angle += 360.0f;
	}
	m_Rotation.z = angle;
	resetLocationMatrix();
}

void	Node::mergeMesh(std::vector<float>& vertices, std::vector<int>& indices, const glm::mat4& parentMatrix)
{
	glm::mat4 matP = parentMatrix * m_LocationMatrix;
	if (m_Mesh) {
		int startV = (int)vertices.size() / 8;

		glm::mat4 matN = glm::transpose(glm::inverse(matP));

		int vcnt = m_Mesh->m_VerticesCount;
		float* vertex = m_Mesh->m_Vertices;
		for (int i = 0; i < vcnt; i++) {
			glm::vec4 v(vertex[0], vertex[1], vertex[2], 1.0f);
			vertex += 3;

			v = matP * v;
			vertices.push_back(v.x);
			vertices.push_back(v.y);
			vertices.push_back(v.z);

			glm::vec4 n(vertex[0], vertex[1], vertex[2], 0.0f);
			vertex += 3;

			n = glm::normalize(matN * n);
			vertices.push_back(n.x);
			vertices.push_back(n.y);
			vertices.push_back(n.z);

			vertices.push_back(vertex[0]);
			vertices.push_back(vertex[1]);

			vertex += 2;
		}

		int icnt = m_Mesh->m_IndicesCount;
		int* index = m_Mesh->m_Indices;
		for (int i = 0; i < icnt; i++) {
			indices.push_back(index[0] + startV);
			index++;
		}
	}
	for (Node* n : m_Children) {
		n->mergeMesh(vertices, indices, matP);
	}
}

std::string Node::meshString()
{
	std::vector<float> vertices;
	std::vector<int> indices;
	glm::mat4 parentMatrix(1);
	mergeMesh(vertices, indices, parentMatrix);

	int vcnt = (int)vertices.size() / 8;
	float* vertex = vertices.data();

	std::stringstream ss;
	ss << "float vertices[] = {\n";
	ss << "    //pos                                        //normal                                     //uv\n";
	for (int i = 0; i < vcnt; i++) {
		ss << "    ";
		for (int j = 0; j < 8; j++) {
			std::stringstream sfloat;
			sfloat.flags(std::ios::fixed);
			sfloat << *vertex++ << "f,";


			ss.width(15);
			ss.flags(std::ios::left);
			ss << sfloat.str();
		}
		ss << "\n";
	}
	ss << "};\n";


	int icnt = (int)indices.size();
	int* index = indices.data();


	ss << "int indices[] = {\n";
	for (int i = 0; i < icnt; i += 3) {
		ss << "    " << index[i] << "," << index[i + 1] << "," << index[i + 2] << ",\n";
	}
	ss << "};\n";

	return ss.str();

}
