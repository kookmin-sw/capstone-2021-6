#include <iostream>
#include <vector>
#include <string>

#include "Renderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "FBO.h"

Renderer::Renderer() :
	m_winWidth(0),
	m_winHeight(0),

	m_quadVAO(0),
	m_quadVBO(0),

	m_lightPos(0),
	m_lightColor(0),
	m_objPos(0),

	m_geometryShader(nullptr),
	m_lightShader(nullptr),

	m_pbrShader(nullptr),
	m_cubemapShader(nullptr),
	m_irradianceShader(nullptr),
	m_backgroundShader(nullptr),

	m_FBO(nullptr),

	m_mainModel(nullptr),

	m_camera(nullptr)
{
}

// Set up Framebuffer object, Shader, Main Object and Screen Fill Quad
void Renderer::StartRenderer(unsigned int width, unsigned int height)
{
	m_winWidth = width;
	m_winHeight = height;

	m_geometryShader = new Shader("../Resources/Shaders/GeometryPass.vs", "../Resources/Shaders/GeometryPass.fs");
	m_lightShader = new Shader("../Resources/Shaders/LightPass.vs", "../Resources/Shaders/LightPass.fs");
	
	m_pbrShader = new Shader("../Resources/Shaders/PBR.vs", "../Resources/Shaders/PBR.fs");
	m_cubemapShader = new Shader("../Resources/Shaders/Cube.vs", "../Resources/Shaders/Cube.fs");
	m_irradianceShader = new Shader("../Resources/Shaders/Cube.vs", "../Resources/Shaders/Irradiance.fs");
	m_backgroundShader = new Shader("../Resources/Shaders/Background.vs", "../Resources/Shaders/Background.fs");

	m_FBO = new FBO(width, height);
	m_FBO->Init();

	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
	m_mainModel = new Model("../Resources/resources/objects/backpack/backpack.obj");
	
	m_lightShader->UseProgram();
	m_lightShader->SetInt("posData", 0);
	m_lightShader->SetInt("normalData", 1);
	m_lightShader->SetInt("albedoData", 2);
	m_lightShader->SetFloat("ao", 1.0f);

	m_pbrShader->UseProgram();
	m_pbrShader->SetInt("irradianceMap", 0);
	m_pbrShader->SetInt("albedoData", 1);
	m_pbrShader->SetFloat("ao", 1.0f);

	m_backgroundShader->UseProgram();
	m_backgroundShader->SetInt("environmentMap", 0);

	glm::vec3 objPos = glm::vec3(0.0f, 0.0f, 0.0f);

	m_lightPos.push_back(glm::vec3( 10.0f, 10.0f, 10.0f));
	m_lightPos.push_back(glm::vec3( 10.0f,-10.0f, 10.0f));
	m_lightPos.push_back(glm::vec3(-10.0f, 10.0f, 10.0f));
	m_lightPos.push_back(glm::vec3(-10.0f, 10.0f, 10.0f));

	m_lightColor.push_back(glm::vec3(30.0f, 19.0f, 10.0f));
	m_lightColor.push_back(glm::vec3(30.0f, 19.0f, 10.0f));
	m_lightColor.push_back(glm::vec3(30.0f, 19.0f, 10.0f));
	m_lightColor.push_back(glm::vec3(30.0f, 19.0f, 10.0f));

	float quadVertices[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);
	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glGenFramebuffers(1, &m_captureFBO);
	glGenRenderbuffers(1, &m_captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRBO);
}

// Main Rendering Part
void Renderer::DeferredRendering()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_FBO->SetFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(glm::radians(m_camera->m_ZOOM), (float)m_winWidth / (float)m_winHeight, 0.1f, 100.0f);
	glm::mat4 view = m_camera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	m_geometryShader->UseProgram();
	m_geometryShader->SetMat4("projMatrix", projection);
	m_geometryShader->SetMat4("viewMatrix", view);

	model = glm::mat4(1.0f);
	model = glm::translate(model, m_objPos);
	model = glm::scale(model, glm::vec3(0.5f));
	m_geometryShader->SetMat4("modelMatrix", model);
	m_mainModel->Draw(*m_geometryShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_lightShader->UseProgram();
	m_FBO->SetTexture();

	m_lightShader->SetVec3("camPos", m_camera->GetPos());
	m_lightShader->SetFloat("metallic", 0.4f);
	m_lightShader->SetFloat("roughness", 0.4f);

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	for (unsigned int i = 0; i < m_lightPos.size(); i++)
	{

		m_lightShader->SetVec3("light[" + std::to_string(i) + "].Position", m_lightPos[i]);
		m_lightShader->SetVec3("light[" + std::to_string(i) + "].Color", m_lightColor[i]);

		model = glm::mat4(1.0f);
		model = glm::translate(model, m_lightPos[i]);
		model = glm::scale(model, glm::vec3(0.5f));
		
	}
}

void Renderer::ImageBasedLighing()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pbrShader->UseProgram();
	glm::mat4 view = m_camera->GetViewMatrix();
	m_pbrShader->SetMat4("viewMatrix", view);
	m_pbrShader->SetVec3("camPos", m_camera->GetPos());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);

	glm::mat4 model = glm::mat4(1.0f);

	m_pbrShader->SetFloat("metallic", 0.4f);
	m_pbrShader->SetFloat("roughness", 0.4f);
	m_pbrShader->SetMat4("modelMatrix", model);

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	for (unsigned int i = 0; i < m_lightPos.size(); i++)
	{

		m_lightShader->SetVec3("light[" + std::to_string(i) + "].Position", m_lightPos[i]);
		m_lightShader->SetVec3("light[" + std::to_string(i) + "].Color", m_lightColor[i]);

		model = glm::mat4(1.0f);
		model = glm::translate(model, m_lightPos[i]);
		model = glm::scale(model, glm::vec3(0.5f));

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
}

void Renderer::processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_camera->ProcessKeyBoard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_camera->ProcessKeyBoard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_camera->ProcessKeyBoard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_camera->ProcessKeyBoard(RIGHT, deltaTime);
}