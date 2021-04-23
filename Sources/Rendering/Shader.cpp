#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.h"

Shader::Shader(const char* vsPath, const char* fsPath, const char* gsPath)
{
    std::string vsCode;
    std::string fsCode;
    std::string gsCode;
    std::ifstream vsFile;
    std::ifstream fsFile;
    std::ifstream gsFile;

    vsFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fsFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    gsFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vsFile.open(vsPath);
        fsFile.open(fsPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vsFile.rdbuf();
        fShaderStream << fsFile.rdbuf();		

        vsFile.close();
        fsFile.close();

        vsCode = vShaderStream.str();
        fsCode = fShaderStream.str();			

        if(gsPath != nullptr)
        {
            gsFile.open(gsPath);
            std::stringstream gShaderStream;
            gShaderStream << gsFile.rdbuf();
            gsFile.close();
            gsCode = gShaderStream.str();
        }        
    }

    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vShaderCode = vsCode.c_str();
    const char * fShaderCode = fsCode.c_str();

    unsigned int vertex, fragment;
    int success = 0;
    char compileLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(vertex, 512, nullptr, compileLog);
		std::cout << "Failed to compile vertex shader: " << compileLog << std::endl;
	}

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);    
	if (success == 0)
	{
		glGetShaderInfoLog(fragment, 512, nullptr, compileLog);
		std::cout << "Failed to compile fragment shader: " << compileLog << std::endl;
	}

    unsigned int geometry;
    if(gsPath != nullptr)
    {
        const char * gShaderCode = gsCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if (success == 0)
	    {
		    glGetShaderInfoLog(geometry, 512, nullptr, compileLog);
		    std::cout << "Failed to compile geometry shader: " << compileLog << std::endl;
	    }
    }

    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    if(gsPath != nullptr)
        glAttachShader(m_id, geometry);
    glLinkProgram(m_id);
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(m_id, 512, nullptr, compileLog);
		std::cout << "Failed to link shader program: " << compileLog << std::endl;
	}

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if(gsPath != nullptr)
        glDeleteShader(geometry); 
}

void Shader::UseProgram()
{
    glUseProgram(m_id);
}

void Shader::SetBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::SetVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]); 
}

void Shader::SetVec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z); 
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
{ 
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]); 
}
void Shader::SetVec4(const std::string &name, float x, float y, float z, float w) 
{ 
    glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w); 
}

void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
