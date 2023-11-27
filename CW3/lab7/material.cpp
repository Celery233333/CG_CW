#include "material.h"

Material::Material()
{
	m_RefCount = 1;
	m_Ka = glm::vec3(1);
	m_Kd = glm::vec3(1);
	m_Ks = glm::vec3(0.3);
	m_Ns = 32;
}

Material::~Material()
{

}

void	Material::addRef()
{
	m_RefCount++;
}
void	Material::release()
{
	m_RefCount--;
	if (m_RefCount <= 0) {
		delete this;
	}
}
