#pragma once
#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Shader;
class Model;
class Camera;
class GBuffer;
class Renderer
{
public:
    Renderer();

	~Renderer();

	void StartRenderer(unsigned int width, unsigned int height);

	void GeometryPass();

	void ShadowPass();

	void LightPass();

	void RenderScene(Shader& shader);

	void RenderQuad();

	void processInput(GLFWwindow* window, float deltaTime);

	unsigned int LoadTexture(char const* path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;
			
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

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

	bool m_bShadows;

	std::vector<glm::vec3> m_lightPos;
	std::vector<glm::vec3> m_lightColor;
	glm::vec3 m_objPos;
	glm::vec3 m_planePos;

	Shader* m_geometryShader;
	Shader* m_lightShader;
	Shader* m_shadowShader;

	GBuffer* m_GBuffer;

	Model* m_mainModel;

	Camera* m_camera;
};