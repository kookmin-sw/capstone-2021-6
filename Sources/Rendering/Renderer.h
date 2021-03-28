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

    int main();

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

	glm::vec3 m_lightPos;
	glm::vec3 m_lightColor;

	Shader* m_geometryShader;
	Shader* m_lightShader;

	Model* m_mainModel;

	Camera* m_camera;
};