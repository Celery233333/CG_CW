#pragma once
#include <glm/glm.hpp>

class Material
{
public:
			Material();
			~Material();
			void	addRef();
			void	release();
public:
	glm::vec3 m_Ka;
	glm::vec3 m_Kd;
	glm::vec3 m_Ks;
	float m_Ns;
private:
	int	m_RefCount;
};

