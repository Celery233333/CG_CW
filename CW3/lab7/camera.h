#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
			Camera();
			~Camera();
	void	init(glm::vec3 center, float pitch, float yaw, float distance);
	void	resetViewMatrix();
	void	resetProjectionMatrix(int width, int height);
	void	startMouseRotate();
	void	onMouseRotate(int dx, int dy);
	void	zoom(int zDelta);
	const glm::mat4& getProjectionMatrix() { return m_ProjectionMatrix; }
	const glm::mat4& getViewMatrix() { return m_ViewMatrix; }
	const glm::vec3& getPosition() { return m_Position; }
	void	moveFrontBack(double dist);
	void	moveLeftRight(double dist);
	void	setCenter(const glm::vec3& pos);
private:
	glm::vec3		m_Position;//eye position
	glm::vec3		m_Center;//look at location
	float m_Pitch;//The angle of rotation around the x-axis
	float m_Yaw;//The angle of rotation around the y-axis
	float m_Distance;//Distance between center and position

	glm::mat4	m_ViewMatrix;//camera matrix
	glm::mat4	m_ProjectionMatrix;//perspective projection matrix


	float	m_BeginPitch;//m_Pitch when the right mouse button is pressed
	float	m_BeginYaw;//m_Yaw when the right mouse button is pressed

};

extern Camera* g_camera;