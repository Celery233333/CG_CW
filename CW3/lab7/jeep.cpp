#include "jeep.h"
#include "light.h"
#include "curve.h"
Jeep* g_Jeep = NULL;

Jeep::Jeep()
{
	m_IsMoving = false;
	m_MovingTime = 0.0;
}
Jeep::~Jeep()
{

}
Node* Jeep::createWheel(Material* mtl, float x,float z,float wheelWidth,float angle)
{
	Texture* wheelTextures[2] = {
	Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
	Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
	};

	Node* cylinder = new Node();
	cylinder->createCylinder(mtl, wheelTextures);
	cylinder->setScale(glm::vec3(m_WheelRadius*2, wheelWidth, m_WheelRadius*2));
	cylinder->setRotation(glm::vec3(angle, 0, 0));
	cylinder->resetLocationMatrix();

	Node* wheel = new Node();
	wheel->addChildNode(cylinder);
	wheel->setPosition(glm::vec3(x, m_WheelRadius, z));
	wheel->resetLocationMatrix();
	addChildNode(wheel);
	return wheel;
}

Node* Jeep::createLight(Material* mtl, float x, float y, float z, float radius, float width, Light** pLight)
{
	Node* node = new Node();

	Texture* lampshadeTextures[2] = {
	Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
	Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
	};

	Node* cylinder = new Node();
	cylinder->createCylinder(mtl, lampshadeTextures);
	cylinder->setScale(glm::vec3(radius*2.0f, width, radius*2.0f));
	cylinder->setRotation(glm::vec3(0, 0, -90));
	cylinder->resetLocationMatrix();
	node->addChildNode(cylinder);

	Texture* luminousTextures[2] = {
	Texture::getTexture(TEXTURE_ID_LIGHT),
	Texture::getTexture(TEXTURE_ID_LIGHT),
	};

	cylinder = new Node();
	cylinder->createCylinder(mtl, luminousTextures);
	cylinder->setScale(glm::vec3(radius * 2.0f, 0.01f, radius * 2.0f));
	cylinder->setRotation(glm::vec3(0, 0, 90));
	cylinder->resetLocationMatrix();

	Light* light = Light::createSpotLight(glm::vec3(1, 1, 1), 1.5f, 0.05f, 0.02f, 30.0f);
	light->setRotation(glm::vec3(0, 0, 120));
	light->resetLocationMatrix();

	node->setPosition(glm::vec3(x, y, z));
	node->resetLocationMatrix();

	node->addChildNode(light);
	node->addChildNode(cylinder);
	addChildNode(node);

	*pLight = light;
	return node;
}

void Jeep::create()
{
	float length = 3.0f;
	float width = 1.2f;
	float height = 0.5f;
	m_WheelRadius = 0.4f;
	float wheelWidth = 0.25f;
	float wheelZ = width / 2;
	float frontWheelX = -0.65f;
	float backWheelX = 0.9f;

	Material* mtl = new Material;

	m_WheelNode[0] = createWheel(mtl, frontWheelX, wheelZ,wheelWidth, 90);
	m_WheelNode[1] = createWheel(mtl, backWheelX, wheelZ, wheelWidth, 90);
	m_WheelNode[2] = createWheel(mtl, frontWheelX, -wheelZ, wheelWidth, -90);
	m_WheelNode[3] = createWheel(mtl, backWheelX, -wheelZ, wheelWidth, -90);


	Texture* chassisTextures[5] = {
		Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
		Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
		Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
		Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
		Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
	};

	Node* chassis = new Node();
	chassis->createCube(mtl, chassisTextures);
	chassis->setPosition(glm::vec3(0, m_WheelRadius, 0));
	chassis->setScale(glm::vec3(length, height, width));
	chassis->resetLocationMatrix();
	addChildNode(chassis);


	Texture* bodyTextures[5] = {
	Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
	Texture::getTexture(TEXTURE_ID_JEEP_SIDE_WINDOW),
	Texture::getTexture(TEXTURE_ID_JEEP_SIDE_WINDOW),
	Texture::getTexture(TEXTURE_ID_JEEP_FRONT_WINDOW),
	Texture::getTexture(TEXTURE_ID_JEEP_FRONT_WINDOW),
	};

	float bodylength = 2.0f;
	float bodyHeight = 0.7f;
	float bodyX = length * 0.5f - bodylength + bodylength * 0.5f;
	float bodyY = height + m_WheelRadius;
	Node* body = new Node();
	body->createCube(mtl, bodyTextures);
	body->setPosition(glm::vec3(bodyX, bodyY, 0));
	body->setScale(glm::vec3(bodylength, bodyHeight, width));
	body->resetLocationMatrix();
	addChildNode(body);

	//Create two headlights
	float lightRadius = 0.1f;
	float lightLength = 0.2f;
	float lightX = -length / 2 + 0.05f;
	float lightY = height + m_WheelRadius + lightRadius;
	float lightZ = width / 2 - 0.2f;
	createLight(mtl, lightX, lightY, lightZ, lightRadius, lightLength, &m_Lights[0]);
	createLight(mtl, lightX, lightY, -lightZ, lightRadius, lightLength, &m_Lights[1]);

	m_IsMoving = true;
	update(0);
	update(0.001);
	m_IsMoving = false;
	mtl->release();
}

bool Jeep::update(double dt)
{
	if (!m_IsMoving) return false;
	m_MovingTime += dt;
	double roundTime = 30;
	float t = (float)(m_MovingTime / roundTime);
	while (t > 1.0) {
		t -= 1.0;
	}
	
	glm::vec3 pos;
	g_Curve->getPosition(t, pos);
	pos.y = 0;

	glm::vec3 dir = pos - m_Position;
	float dist = glm::length(dir);
	float wheelAngle = dist * 360.0f / (2.0f * (float)M_PI * m_WheelRadius);
	for (int i = 0; i < 4; i++) {
		m_WheelNode[i]->rotateZ(wheelAngle);
	}

	dir = dir / dist;
	float angle = glm::degrees(glm::acos(glm::dot(dir, glm::vec3(-1, 0, 0))));
	if (dir.z < 0.0f) {
		angle = -angle;
	}
	setPosition(pos);
	setRotation(glm::vec3(0, angle, 0));
	resetLocationMatrix();
	return true;
}

void Jeep::switchMove()
{
	m_IsMoving = !m_IsMoving;
}

void Jeep::turnLight()
{
	m_Lights[0]->turn();
	m_Lights[1]->turn();
}
