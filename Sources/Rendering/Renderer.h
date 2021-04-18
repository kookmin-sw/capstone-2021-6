#pragma once
#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Shader;
class Model;
class Camera;
class FBO;
class Renderer
{
public:
    Renderer();

	void StartRenderer(unsigned int width, unsigned int height);

    void DeferredRendering();

	void processInput(GLFWwindow* window, float deltaTime);

private:
	unsigned int m_winWidth;
	unsigned int m_winHeight;

	unsigned int m_shadowWidth;
	unsigned int m_shadowHeight;

	unsigned int m_quadVAO;
	unsigned int m_quadVBO;

	unsigned int m_planeVAO;
	unsigned int m_planeVBO;

	unsigned int m_depthMapFBO;
	unsigned int m_depthCubemap;

	std::vector<glm::vec3> m_lightPos;
	std::vector<glm::vec3> m_lightColor;
	glm::vec3 m_objPos;
	glm::vec3 m_planePos;

	Shader* m_geometryShader;
	Shader* m_lightShader;
	Shader* m_shadowShader;

	FBO* m_FBO;

	Model* m_mainModel;

	Camera* m_camera;
};