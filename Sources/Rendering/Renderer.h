#pragma once
#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Shader;
class Model;
class Camera;
class Renderer
{
public:
    Renderer();

	void StartRenderer(unsigned int width, unsigned int height);

    void DeferredRendering();

private:
	unsigned int m_winWidth;
	unsigned int m_winHeight;

	unsigned int m_quadVAO;
	unsigned int m_quadVBO;

	unsigned int m_gBuffer;
	unsigned int m_posData;
	unsigned int m_normalData;
	unsigned int m_albedoData;
	unsigned int m_depth;

	std::vector<glm::vec3> m_lightPos;
	std::vector<glm::vec3> m_lightColor;
	std::vector<glm::vec3> m_objPos;

	Shader* m_geometryShader;
	Shader* m_lightShader;

	Model* m_mainModel;

	Camera* m_camera;
};