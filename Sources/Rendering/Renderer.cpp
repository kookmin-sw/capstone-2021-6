#include <iostream>
#include <vector>
#include <string>

#include "Renderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

Camera* camera = new Camera();
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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

	m_geometryShader(nullptr),
	m_lightShader(nullptr),

	m_mainModel(nullptr),

	m_camera(nullptr)
{
	srand(13);
	float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
	float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
	float zPos = ((rand() % 100) / 100.0) * 6.0 - 5.0;
	m_lightPos = glm::vec3(xPos, yPos, zPos);

	float rColor = ((rand() % 100) / 200.0f) + 0.5;
	float gColor = ((rand() % 100) / 200.0f) + 0.5;
	float bColor = ((rand() % 100) / 200.0f) + 0.5;
	m_lightColor = glm::vec3(rColor, gColor, bColor);

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
}

int Renderer::main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(m_winWidth, m_winHeight, "Capstone", nullptr, nullptr);
	if (window == NULL)
	{
		std::cout << "Failed to Create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (gl3wInit())
	{
		fprintf(stderr, "failed to initialize OpenGL\n");
		return -1;
	}

	m_camera = camera;

	Shader geometry("../../Resources/Shaders/GeometryPass.vs", "../../Resources/Shaders/GeometryPass.fs");
	Shader light("../../Resources/Shaders/LightPass.vs", "../../Resources/Shaders/LightPass.fs");
	m_geometryShader = &geometry;
	m_lightShader = &light;

	Model model("../../Resources/resources/objects/backpack/backpack.obj");
	m_mainModel = &model;

	m_lightShader->UseProgram();
	m_lightShader->SetInt("posData", 0);
	m_lightShader->SetInt("normalData", 1);
	m_lightShader->SetInt("albedoData", 2);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DeferredRendering();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void Renderer::DeferredRendering()
{
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

	glGenRenderbuffers(1, &m_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_winWidth, m_winHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer isn't complete" << std::endl;
	}

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(glm::radians(m_camera->m_ZOOM), (float)m_winWidth / (float)m_winHeight, 0.1f, 100.0f);
	glm::mat4 view = m_camera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	m_geometryShader->UseProgram();
	m_geometryShader->SetMat4("projMatrix", projection);
	m_geometryShader->SetMat4("viewMatrix", view);
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

	m_lightShader->SetVec3("light.Position", m_lightPos);
	m_lightShader->SetVec3("light.Color", m_lightColor);
	m_lightShader->SetVec3("viewPos", m_camera->GetPos());

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}