#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

enum ECameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
public:
	static const float m_YAW;
	static const float m_PITCH;
	static const float m_SPEED;
	static const float m_SENSITIVITY;
	static const float m_ZOOM;

	Camera(
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f),
		float yawAngel = m_YAW,
		float pitchAngle = m_PITCH);

	glm::mat4 GetViewMatrix();

	glm::vec3 GetPos();

	void SetPos(glm::vec3 pos);

	void ProcessKeyBoard(ECameraMovement direction, float deltaTime);

	void ProcessMouse(float xOffset, float yOffset, bool constrainPitch = true);

private:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	float m_yaw;
	float m_pitch;

	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

	void UpdateCameraVectors();
};