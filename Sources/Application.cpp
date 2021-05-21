#include "Application.h"
#include "Renderer.h"
#include "Scene.h"
#include "Viewport.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <chrono>
#include <iostream>

Application::Application(const std::string& title, unsigned int width, unsigned int height, bool bFullscreen) :
	m_bIsRunning(false),
	m_title(title),
	m_scene(nullptr),
	m_renderer(nullptr),
	m_windowWidth(width),
	m_windowHeight(height),
   m_bFullScreen(bFullscreen)
{
}

Application::~Application()
{
	if (m_renderer != nullptr)
	{
		delete m_renderer;
		m_renderer = nullptr;
	}
}

bool Application::InitBase()
{
	m_renderer = new Renderer();

	if (!m_renderer->Init(m_windowWidth, m_windowHeight))
	{
		std::cout << "Failed to init renderer" << std::endl;
		return false;
	}

	return true;
}

bool Application::InitWindows()
{
	glfwInit();
	const char* glslVersion = "#version 450 core";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	if (m_bFullScreen)
	{
		const auto monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		m_window = glfwCreateWindow(m_windowWidth, m_windowHeight,
			m_title.c_str(), monitor, nullptr);
	}
	else
	{
		m_window = glfwCreateWindow(m_windowWidth, m_windowHeight,
			m_title.c_str(), nullptr, nullptr);
	}


	if (m_window == nullptr)
	{
		std::cout << "Failed to init glfw window" << std::endl;
		return false;
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(0);

	/* Initialize Callbacks */
	glfwSetWindowUserPointer(m_window, this);

	const auto keyCallback = [](GLFWwindow * window, int key, int scanCode, int action, int mods) {
		auto* app = (Application*)glfwGetWindowUserPointer(window);
		app->_KeyCallback(window, key, scanCode, action, mods); };
	glfwSetKeyCallback(m_window, keyCallback);

	const auto framebufferSizeCallback = [](GLFWwindow * window, int width, int height) {
		auto* app = (Application*)glfwGetWindowUserPointer(window);
		app->_WindowResizeCallback(window, width, height);
	};
	glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

	int err = gl3wInit();
	int version = gl3wIsSupported(3, 3);

	return true;
}

bool Application::InitImGui(GLFWwindow* window)
{
	const char* glslVersion = "#version 450 core";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);

	return true;
}

void Application::ControlImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Control Scene", 0, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close", "Ctrl+W")) { this->Stop(); }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::NewLine();

		ImGui::Checkbox("Conservative Rasterization", &m_renderer->bEnableConservativeRasterization);
		ImGui::Checkbox("Always Voxelize", &m_renderer->bAlwaysVoxelize);
		ImGui::Checkbox("Enable Direct Diffuse", &m_renderer->bEnableDirectDiffuse);
		ImGui::Checkbox("Enable Indirect Diffuse", &m_renderer->bEnableIndirectDiffuse);
		ImGui::Checkbox("Enable Direct Specular", &m_renderer->bEnableDirectSpecular);
		ImGui::Checkbox("Enable Indirect Specular", &m_renderer->bEnableIndirectSpecular);
		ImGui::Checkbox("Enable ViewFrustum Culling", &m_renderer->bEnableViewFrustumCulling);

		ImGui::End();
	}

	{
		ImGui::Begin("Keyboard Tooltip");
		ImGui::BulletText("Program Exit : Escape");
		ImGui::BulletText("UI On / Off : U");
		ImGui::BulletText("Camera Movement : W S A D");
		ImGui::BulletText("Light Movement : UpArrow DownArrow LeftArrow RightArrow");
		ImGui::BulletText("Enable CamPath : C");
		ImGui::BulletText("Enable Light Rotation : L");
		ImGui::NewLine();

		ImGui::BulletText("Plus Floor Roughness : Plus");
		ImGui::BulletText("Minus Floor Roughness : Minus");
		ImGui::BulletText("Change RenderMode : LeftBracket RightBracket");
		ImGui::BulletText("Scene Change to Sponza : F5");
		ImGui::BulletText("Scene Change to CornellBox : F6");
		ImGui::NewLine();

		ImGui::BulletText("VCT Specular SampleNum * 2 : PageUp");
		ImGui::BulletText("VCT Specular SampleNum / 2 : PageDown");

		ImGui::End();
	}

	{
		ImGui::Begin("Debugging");

		ImGui::Checkbox("Debug Cone Direction", &m_renderer->bDebugConeDirection);
		ImGui::Checkbox("Debug BoundingBox", &m_renderer->bDebugBoundingBox);
		ImGui::Checkbox("Debug AmbientOcclusion", &m_renderer->bDebugAmbientOcclusion);

		ImGui::End();
	}
}

int Application::Run()
{
	if (!InitWindows())
	{
		return 1;
	}
	if (!InitBase())
	{
		return 1;
	}
	if (!Init())
	{
		return 1;
	}
	if (!InitImGui(GetWindow()))
	{
		return 1;
	}

	m_bIsRunning = true;
	float deltaTime = 0.0f;
	while (m_bIsRunning && !glfwWindowShouldClose(m_window))
	{
		auto begin = std::chrono::system_clock::now();
		glfwPollEvents();
		ControlImGui();

		Update(deltaTime);

		if (m_scene != nullptr)
		{
			m_scene->Update(deltaTime);
			m_renderer->Render(m_scene);
			m_scene->ResolveDirty();
		}
		
		if (m_renderer->bEnableUI)
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(m_window);

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> dt = (end - begin);
		deltaTime = dt.count();
	}

	glfwTerminate();
	return 0;
}

void Application::_WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;
	WindowResizeCallback(window, width, height);
}

void Application::_KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	KeyCallback(window, key, scanCode, action, mods);
	m_scene->KeyCallback(window, key, scanCode, action, mods);
}