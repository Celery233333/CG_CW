#pragma once
#include "node.h"

enum {
	LIGHT_TYPE_DIRECTION,
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_SPOT,
};

class Light : public Node
{
public:
			Light(int type);
virtual	~Light();
static	Light* createDirectionalLight(glm::vec3 dir, glm::vec3 color);
static	Light* createPointLight(glm::vec3 color, float constAttenuation, float linearAttenuation, float quadraticAttenuation);
static	Light* createSpotLight(glm::vec3 color, float constAttenuation, float linearAttenuation, float quadraticAttenuation,float cutoff);
virtual	void	resetWorldMatrix();

	void	setLightPosition(glm::vec3& pos) { m_LightPosition = pos; }
	void	setColor(glm::vec3& c) { m_Color = c; }
	void	setAttenuation(float constAttenuation, float linearAttenuation, float quadraticAttenuation)
	{
		m_ConstAttenuation = constAttenuation;;
		m_LinearAttenuation = linearAttenuation;
		m_QuadraticAttenuation = quadraticAttenuation;
	}
	void	setSpotCutoff(float cutoff) { m_SpotCutoff = cutoff; }
	bool	isOn() { return m_IsOn; }
	void	turn() { m_IsOn = !m_IsOn; }

	const glm::vec3& getLightPosition() { return m_LightPosition; }
	int	getType() { return m_Type; }
	const glm::vec3& getColor() { return m_Color; }
	float getConstAttenuation() { return m_ConstAttenuation; }
	float getLinearAttenuation() { return m_LinearAttenuation; }
	float getQuadraticAttenuation() { return m_QuadraticAttenuation; }
	float	getSpotCutoff() { return m_SpotCutoff; }
	const glm::vec3& getSpotDirection() { return m_SpotDirection; }
private:
	bool	m_IsOn;
	int	m_Type;
	glm::vec3	m_Color;
	glm::vec3	m_LightPosition;
	glm::vec3	m_SpotDirection;

	float m_ConstAttenuation;
	float m_LinearAttenuation;
	float m_QuadraticAttenuation;
	float	m_SpotCutoff;
};

extern std::vector<Light*>	g_Lights;