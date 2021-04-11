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

	m_shadowWidth(0),
	m_shadowHeight(0),

	m_quadVAO(0),
	m_quadVBO(0),

	m_planeVAO(0),
	m_planeVBO(0),

	m_depthMapFBO(0),
	m_depthCubemap(0),

	m_captureFBO(0),
	m_captureRBO(0),

	m_hdrTexture(0),

	m_irradianceMap(0),
	m_envCubemap(0),

	m_lightPos(0),
	m_lightColor(0),
	m_objPos(0),

	m_geometryShader(nullptr),
	m_lightShader(nullptr),
	m_shadowShader(nullptr),

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
	// Set Parameters
	m_winWidth = width;
	m_winHeight = height;

	m_geometryShader = new Shader("../Resources/Shaders/GeometryPass.vs", "../Resources/Shaders/GeometryPass.fs");
	m_lightShader = new Shader("../Resources/Shaders/LightPass.vs", "../Resources/Shaders/LightPass.fs");
	m_shadowShader = new Shader("../Resources/Shaders/Shadow.vs", "../Resources/Shaders/Shadow.fs", "../Resources/Shaders/Shadow.gs");
	
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
	m_lightShader->SetInt("depthMap", 3);
	m_lightShader->SetFloat("ao", 1.0f);

	m_pbrShader->UseProgram();
	m_pbrShader->SetInt("irradianceMap", 0);
	m_pbrShader->SetInt("albedoData", 1);
	m_pbrShader->SetFloat("ao", 1.0f);

	m_backgroundShader->UseProgram();
	m_backgroundShader->SetInt("environmentMap", 0);

	// Set Objects
	glm::vec3 objPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 planePos = glm::vec3(0.0f, 0.0f, 0.0f);
	m_objPos = objPos;
	m_planePos = planePos;

	m_lightPos.push_back(glm::vec3(0.0f, 3.0f, 0.0f));
	m_lightPos.push_back(glm::vec3(0.0f, 3.0f, 0.0f));
	m_lightPos.push_back(glm::vec3(0.0f, 3.0f, 0.0f));
	m_lightPos.push_back(glm::vec3(0.0f, 3.0f, 0.0f));

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

	float planeVertices[] = {
		 5.0f, -5.0f,  5.0f, 25.0f,  0.0f,
		-5.0f, -5.0f,  5.0f,  0.0f,  0.0f,
		-5.0f, -5.0f,  5.0f,  0.0f, 25.0f,

		 5.0f, -5.0f,  5.0f, 25.0f,  0.0f,
		-5.0f, -5.0f,  5.0f,  0.0f, 25.0f,
		 5.0f, -5.0f,  5.0f, 25.0f, 25.0f
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
	glBindVertexArray(0);

	glGenVertexArrays(1, &m_planeVAO);
	glGenBuffers(1, &m_planeVBO);
	glBindVertexArray(m_planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// Set Shadow
	const unsigned int shadowWidth = 5120, shadowHeight = 5120;
	m_shadowWidth = shadowWidth;
	m_shadowHeight = shadowHeight;

	glGenFramebuffers(1, &m_depthMapFBO);

	glGenTextures(1, &m_depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

// Main Rendering Part
void Renderer::DeferredRendering()
{
	// Set Geometry Pass
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

	model = glm::translate(model, m_planePos);
	model = glm::scale(model, glm::vec3(0.5f));
	m_geometryShader->SetMat4("modelMatrix", model);
	glBindVertexArray(m_planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Set Shadow Pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float near_plane = 0.1f, far_plane = 100.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)m_shadowWidth / (float)m_shadowHeight, near_plane, far_plane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos[0], m_lightPos[0] + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos[0], m_lightPos[0] + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos[0], m_lightPos[0] + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos[0], m_lightPos[0] + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos[0], m_lightPos[0] + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos[0], m_lightPos[0] + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	glViewport(0, 0, m_shadowWidth, m_shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	m_shadowShader->UseProgram();
	for (unsigned int i = 0; i < 6; ++i)
	{
		m_shadowShader->SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}
	m_shadowShader->SetFloat("far_plane", far_plane);
	m_shadowShader->SetVec3("lightPos", m_lightPos[0]);

	model = glm::mat4(1.0f);
	model = glm::translate(model, m_objPos);
	model = glm::scale(model, glm::vec3(0.5f));
	m_shadowShader->SetMat4("modelMatrix", model);
	m_mainModel->Draw(*m_shadowShader);

	model = glm::translate(model, m_planePos);
	model = glm::scale(model, glm::vec3(0.5f));
	m_shadowShader->SetMat4("modelMatrix", model);
	glBindVertexArray(m_planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set Light Pass
	glViewport(0, 0, m_winWidth, m_winHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_lightShader->UseProgram();
	m_FBO->SetTexture();
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);

	m_lightShader->SetVec3("camPos", m_camera->GetPos());
	m_lightShader->SetFloat("fat_plane", far_plane);
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

	//glBindVertexArray(m_quadVAO);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glBindVertexArray(0);
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
	}
}

void Renderer::processInput(GLFWwindow* window, float deltaTime)
{
	float velocity = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// KEYBOARD W A S D (Camera Movement)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_camera->ProcessKeyBoard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_camera->ProcessKeyBoard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_camera->ProcessKeyBoard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_camera->ProcessKeyBoard(RIGHT, deltaTime);

	// KEYBOARD U H J K (Light Movement)
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < m_lightPos.size(); i++)
			m_lightPos[i].y += velocity;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < m_lightPos.size(); i++)
			m_lightPos[i].y -= velocity;
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < m_lightPos.size(); i++)
			m_lightPos[i].x -= velocity;
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < m_lightPos.size(); i++)
			m_lightPos[i].x += velocity;
	}

	// KEYBOARD F N (Light Distance Movement)
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < m_lightPos.size(); i++)
			m_lightPos[i].z -= velocity;
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < m_lightPos.size(); i++)
			m_lightPos[i].z += velocity;
	}
}