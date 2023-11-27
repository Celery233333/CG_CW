#include "floorLamp.h"
#include "light.h"

FloorLamp::FloorLamp()
{

}

FloorLamp::~FloorLamp()
{

}
Light*	FloorLamp::create()
{
	Texture* lamppostTextures[2] = {
		Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
		Texture::getTexture(TEXTURE_ID_CAMOUFLAGE),
	};

	float height = 1.2f;
	Material* mtl = new Material();
	Node* cylinder = new Node();
	cylinder->createCylinder(mtl, lamppostTextures);
	cylinder->setScale(glm::vec3(0.2f, height, 0.2f));
	cylinder->resetLocationMatrix();

	Node* sphere = new Node();
	sphere->createSphere(mtl, Texture::getTexture(TEXTURE_ID_LIGHT));
	sphere->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
	sphere->setPosition(glm::vec3(0, height, 0));
	Light* light = Light::createPointLight(glm::vec3(1, 1, 1), 1.5f, 0.05f, 0.02f);
	sphere->resetLocationMatrix();

	sphere->addChildNode(light);
	addChildNode(sphere);
	addChildNode(cylinder);
	return light;
}
