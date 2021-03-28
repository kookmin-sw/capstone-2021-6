#include "Application.h"
#include "Renderer.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>

Application::Application(const std::string& title, unsigned int width, unsigned int height) :
	m_title(title),
	m_renderer(nullptr),
	m_wWidth(width),
	m_wHeight(height),
	m_window(nullptr)
{
}

int Application::Run()
{
	Renderer renderer;
	m_renderer = &renderer;
	int res = m_renderer->main();
	return res;
}