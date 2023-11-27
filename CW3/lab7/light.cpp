#include "light.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::vector<Light*>	g_Lights;

Light* Light::createDirectionalLight(glm::vec3 dir, glm::vec3 color)
{
	Light* light = new Light(LIGHT_TYPE_DIRECTION);
	dir = glm::normalize(dir);
	light->setLightPosition(dir);
	light->setColor(color);
	g_Lights.push_back(light);
	return light;
}
Light* Light::createPointLight(glm::vec3 color, float constAttenuation, float linearAttenuation, float quadraticAttenuation)
{
	Light* light = new Light(LIGHT_TYPE_POINT);
	light->setColor(color);
	light->setAttenuation(constAttenuation, linearAttenuation, quadraticAttenuation);
	g_Lights.push_back(light);
	return light;

}

Light* Light::createSpotLight(glm::vec3 color, float constAttenuation, float linearAttenuation, float quadraticAttenuation, float cutoff)
{
	Light* light = new Light(LIGHT_TYPE_SPOT);
	light->setColor(color);
	light->setAttenuation(constAttenuation, linearAttenuation, quadraticAttenuation);
	light->setSpotCutoff(cutoff);
	g_Lights.push_back(light);
	return light;
}

Light::Light(int type)
{
	m_IsOn = true;
	m_Type = type;
	m_Color = glm::vec3(1, 1, 1);
	m_ConstAttenuation = 1.5f;
	m_LinearAttenuation = 0.05f;
	m_QuadraticAttenuation = 0.02f;
	m_SpotCutoff = 180.0f;
	m_SpotDirection = glm::vec3(0, 1, 0);
}

Light::~Light()
{

}

void	Light::resetWorldMatrix()
{
	Node::resetWorldMatrix();
	if (m_Type != LIGHT_TYPE_DIRECTION) {
		m_LightPosition = m_WorldMatrix * glm::vec4(0, 0, 0, 1);

		if (m_Type == LIGHT_TYPE_SPOT) {
			m_SpotDirection = m_WorldMatrix * glm::vec4(0, 1, 0, 0);
			m_SpotDirection = glm::normalize(m_SpotDirection);
		}
	}
}
