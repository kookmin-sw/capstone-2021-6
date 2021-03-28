#include <../../../Sources/Rendering/Camera.h>

#include <glm/vec4.hpp>
#include <glm/gtx/transform.hpp>


const float Camera::m_YAW = -90.0f;
const float Camera::m_PITCH = 0.0f;
const float Camera::m_SPEED = 2.5f;
const float Camera::m_SENSITIVITY = 0.1f;
const float Camera::m_ZOOM = 45.0f;

Camera::Camera(glm::vec3 pos, glm::vec3 upVec, float yawAngle, float pitchAngle) :
	m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(m_SPEED), m_mouseSensitivity(m_SENSITIVITY), m_zoom(m_ZOOM)
{
	m_position = pos;
	m_worldUp = upVec;
	m_yaw = yawAngle;
	m_pitch = pitchAngle;
	UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(m_position, m_position + m_front, m_up);

	//glm::vec4 right{ 1.0f, 0.0f, 0.0f, 0.0f };
	//glm::vec4 up{ 0.0f, 1.0f, 0.0f, 0.0f };
	//glm::vec4 forward{ 0.0f, 0.0f, -1.0f, 0.0f };

	//glm::vec3 position{ m_position };

	//right = glm::normalize(glm::rotate(m_rotation, right));
	//up = glm::normalize(glm::rotate(m_rotation, up));

	//glm::mat4 invTranslation = glm::translate(-position);
	//glm::mat4 invRotation{ right, up, forawrd, glm::vec4() };

	//return (invRotation * invTranslation);
}

glm::vec3 Camera::GetPos()
{
	return m_position;
}

void Camera::ProcessKeyBoard(ECameraMovement direction, float deltaTime)
{
	float velocity = m_movementSpeed * deltaTime;

	if (direction == FORWARD)
	{
		m_position += m_front * velocity;
	}

	if (direction == BACKWARD)
	{
		m_position -= m_front * velocity;
	}

	if (direction == RIGHT)
	{
		m_position += m_right * velocity;
	}

	if (direction == LEFT)
	{
		m_position -= m_right * velocity;
	}
}

void Camera::ProcessMouse(float xOffset, float yOffset, bool constrainPitch)
{
	xOffset *= m_mouseSensitivity;
	yOffset *= m_mouseSensitivity;

	m_yaw += xOffset;
	m_pitch += yOffset;

	if (constrainPitch)
	{
		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		if (m_pitch < -89.0f)
			m_pitch = -89.0f;
	}

	UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 newFront;

	newFront.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	newFront.y = sin(glm::radians(m_pitch));
	newFront.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(newFront);

	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}