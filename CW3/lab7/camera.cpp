#include "Camera.h"
Camera* g_camera = NULL;

Camera::Camera()
{
	m_Pitch = 0.0f;
	m_Yaw = 0.0f;
	m_Distance = 0.0f;
	m_Center = glm::vec3(0, 0, 0);
	m_Position = glm::vec3(0, 0, 0);
	m_BeginPitch = 0.0f;
	m_BeginYaw = 0.0f;
}

Camera::~Camera()
{

}
void	Camera::init(glm::vec3 center, float pitch, float yaw, float distance)
{
	m_Center = center;
	m_Pitch = pitch;
	m_Yaw = yaw;
	m_Distance = distance;
	resetViewMatrix();
}
void	Camera::resetViewMatrix()
{
	glm::mat4 matIdentity(1);
	glm::mat4 matPitch = glm::rotate(matIdentity,glm::radians(m_Pitch), glm::vec3(1, 0, 0));
	glm::mat4 matYaw = glm::rotate(matIdentity,glm::radians(m_Yaw), glm::vec3(0, 1, 0));
	m_Position = m_Center + glm::vec3(matYaw * matPitch * glm::vec4(0, 0, m_Distance, 0));
	m_ViewMatrix = glm::lookAt(m_Position, m_Center, glm::vec3(0, 1, 0));
}

void	Camera::resetProjectionMatrix(int width, int height)
{
	m_ProjectionMatrix = glm::perspective(45.0f, (float)width / (float)height, 0.5f, 5000.0f);
}

void	Camera::startMouseRotate()
{
	m_BeginPitch = m_Pitch;
	m_BeginYaw = m_Yaw;
}

void	Camera::onMouseRotate(int dx, int dy)
{
	float scalar = 0.2f;
	m_Yaw = m_BeginYaw - scalar * dx;
	m_Pitch = m_BeginPitch - scalar * dy;

	if (m_Yaw > 360.0f) {
		m_Yaw -= 360.0f;
	}
	else if (m_Yaw < 0.0f) {
		m_Yaw += 360.0f;
	}

	float minX = -89.0f;
	float maxX = -1.0f;
 	if (m_Pitch > maxX) {
 		m_Pitch = maxX;
 	}
 	else if (m_Pitch < minX) {
 		m_Pitch = minX;
 	}

	resetViewMatrix();
}
void	Camera::zoom(int zDelta)
{
	float scalar = 0.1f;
	if (zDelta > 0) {
		scalar = -scalar;
	}
	m_Distance *= (1.0f + scalar);
	if (m_Distance < 0.01f) {
		m_Distance = 0.01f;
	}
	resetViewMatrix();
}

void	Camera::moveFrontBack(double dist)
{
	glm::vec3 dir = m_Center - m_Position;
	dir.y = 0.0f;
	dir = glm::normalize(dir);
	glm::vec3 offset = dir * (float)dist;
	m_Center += offset;
	resetViewMatrix();
}
void	Camera::moveLeftRight(double dist)
{
	glm::vec3 dir = m_Center - m_Position;
	glm::vec3 right = glm::cross(dir, glm::vec3(0, 1, 0));
	dir = glm::normalize(right);
	glm::vec3 offset = dir * (float)dist;
	m_Center += offset;
	resetViewMatrix();

}

void	Camera::setCenter(const glm::vec3& pos)
{
	m_Center = pos;
	resetViewMatrix();
}
