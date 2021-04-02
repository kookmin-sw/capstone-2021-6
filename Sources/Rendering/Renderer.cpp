#include <iostream>
#include <vector>
#include <string>

#include "Renderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"

Renderer::Renderer() :
	m_winWidth(0),
	m_winHeight(0),

	m_quadVAO(0),
	m_quadVBO(0),

	m_gBuffer(0),
	m_posData(0),
	m_normalData(0),
	m_albedoData(0),
	m_depth(0),

	m_lightPos(0),
	m_lightColor(0),
	m_objPos(0),

	m_geometryShader(nullptr),
	m_lightShader(nullptr),

	m_mainModel(nullptr),

	m_camera(nullptr)
{
}

// Set up Framebuffer object, Shader, Main Object and Screen Fill Quad
void Renderer::StartRenderer(unsigned int width, unsigned int height)
{
	m_winWidth = width;
	m_winHeight = height;

	glGenFramebuffers(1, &m_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

	glGenTextures(1, &m_posData);
	glBindTexture(GL_TEXTURE_2D, m_posData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_winWidth, m_winHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_posData, 0);

	glGenTextures(1, &m_normalData);
	glBindTexture(GL_TEXTURE_2D, m_normalData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_winWidth, m_winHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalData, 0);

	glGenTextures(1, &m_albedoData);
	glBindTexture(GL_TEXTURE_2D, m_albedoData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_winWidth, m_winHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedoData, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &m_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_winWidth, m_winHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_geometryShader = new Shader("../Resources/Shaders/GeometryPass.vs", "../Resources/Shaders/GeometryPass.fs");
	m_lightShader = new Shader("../Resources/Shaders/LightPass.vs", "../Resources/Shaders/LightPass.fs");

	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
	m_mainModel = new Model("../Resources/resources/objects/backpack/backpack.obj");
	
	m_lightShader->UseProgram();
	m_lightShader->SetInt("posData", 0);
	m_lightShader->SetInt("normalData", 1);
	m_lightShader->SetInt("albedoData", 2);
	m_lightShader->SetFloat("ao", 1.0f);

	glm::vec3 objPos = glm::vec3(0.0f, 0.0f, 0.0f);

	m_lightPos.push_back(glm::vec3(-10.0f, 10.0f, 10.0f));
	m_lightPos.push_back(glm::vec3(10.0f, 10.0f, 10.0f));
	m_lightPos.push_back(glm::vec3(-10.0f, -10.0f, 10.0f));
	m_lightPos.push_back(glm::vec3(10.0f, -10.0f, 10.0f));

	m_lightColor.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
	m_lightColor.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
	m_lightColor.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
	m_lightColor.push_back(glm::vec3(300.0f, 300.0f, 300.0f));

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
}

// Main Rendering Part
void Renderer::DeferredRendering()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_posData);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalData);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_albedoData);

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