#pragma once
#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

class Shader
{
	enum class EShaderType
	{
		VertexShader,
		FragmentShader,
		GeometryShader,
		Unknown
	};

public:
    Shader(const char* vsPath, const char* fsPath, const char* gsPath = nullptr);

	unsigned int GetID() const
	{
		return m_id;
	}

	std::string GetPath(EShaderType type) const
	{
		switch (type)
		{
		case EShaderType::VertexShader:
			return m_vsPath;
		case EShaderType::FragmentShader:
			return m_fsPath;
		case EShaderType::GeometryShader:
			return m_gsPath;			
		}

		return "Unknown";
	}


    void LoadAndCompileShaders(const char* vertexPath, const char* fragmentPath);

    void UseProgram();

	void SetBool(const std::string &name, bool value) const;

	void SetInt(const std::string &name, int value) const;

	void SetFloat(const std::string &name, float value) const;

	void SetVec2(const std::string &name, const glm::vec2 &value) const;
	void SetVec2(const std::string &name, float x, float y) const;

	void SetVec3(const std::string &name, const glm::vec3 &value) const;
	void SetVec3(const std::string &name, float x, float y, float z) const;

	void SetVec4(const std::string &name, const glm::vec4 &value) const;
	void SetVec4(const std::string &name, float x, float y, float z, float w);

	void SetMat2(const std::string &name, const glm::mat2 &mat) const;

	void SetMat3(const std::string &name, const glm::mat3 &mat) const;

	void SetMat4(const std::string &name, const glm::mat4 &mat) const;    

private:
    std::string m_vsPath;
    std::string m_fsPath;
	std::string m_gsPath;

    unsigned int m_id;
};