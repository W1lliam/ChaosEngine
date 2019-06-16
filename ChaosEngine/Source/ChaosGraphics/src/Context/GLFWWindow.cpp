#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Context/GLFWWindow.h"
#include "ChaosGraphics/Context/GLFWInput.h"
#include <ChaosDebug.h>

#if defined(CHAOS_OPENGL) && !defined(CHAOS_SHIPPING)
    #include "ChaosGraphics/Renderer/OpenGLRenderer.h"
#endif

namespace Chaos::Graphics
{
    static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int p_error, const char* p_description)	
	{
		CHAOS_CORE_INFO("GLFW ERROR {0}:\n\t{1}", p_error, p_description);
	}

    IWindow* IWindow::Create(const WindowConfiguration& p_configuration)
    {
        return new GLFWWindow(p_configuration);
    }

    GLFWWindow::GLFWWindow(const WindowConfiguration& p_configuration)
    {
        Init(p_configuration);
    }

    GLFWWindow::~GLFWWindow()
    {
        Close();
    }

    void GLFWWindow::OnUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void GLFWWindow::Init(const WindowConfiguration& p_configuration)
    {
        m_data.title = p_configuration.title;
        m_data.width = p_configuration.width;
        m_data.height = p_configuration.height;

        if (!s_GLFWInitialized)
        {
            const int success = glfwInit();
            CHAOS_CORE_ASSERT(success, "Failed to initialize GLFW");
			glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }

#if defined(CHAOS_OPENGL)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifndef CHAOS_SHIPPING
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    #endif
#endif
        m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);

        CHAOS_CORE_ASSERT(m_window, "Failed to create window");

        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(m_window, (mode->width - m_data.width) / 2, (mode->height - m_data.height) / 2);
        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, &m_data);
        SetVSync(false);

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* p_window, int p_width, int p_height) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);
			data.width = p_width;
			data.height = p_height;
			
			Events::EventHandler::Get().Send<Events::WindowResizeEvent>(p_width, p_height);
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* p_window) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);
			Events::EventHandler::Get().Send<Events::WindowsCloseEvent>();
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* p_window, int p_key, int p_keyCode, int p_action, int p_mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);

			switch (p_action)
			{
				case GLFW_PRESS:
				{
					Events::EventHandler::Get().Send<Events::KeyPressedEvent>((E_ChaosKeyCode)p_key, 0);
				}
					break;
				case GLFW_RELEASE:
				{
					Events::EventHandler::Get().Send<Events::KeyReleasedEvent>((E_ChaosKeyCode)p_key);
				}
					break;
				case GLFW_REPEAT:
				{
					Events::EventHandler::Get().Send<Events::KeyPressedEvent>((E_ChaosKeyCode)p_key, 1);
				}
					break;
			}
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* p_window, int p_button, int p_action, int p_mods) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);

			switch (p_action)
			{
				case GLFW_PRESS:
				{
					Events::EventHandler::Get().Send<Events::MouseButtonPressedEvent>((E_ChaosMouseCode)p_button);
				}
					break;
				case GLFW_RELEASE:
				{
					Events::EventHandler::Get().Send<Events::MouseButtonReleasedEvent>((E_ChaosMouseCode)p_button);
				}
					break;
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* p_window, double xOffset, double yOffset) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);
			Events::EventHandler::Get().Send<Events::MouseScrolledEvent>((float)xOffset, (float)yOffset);

		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* p_window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);
			Events::EventHandler::Get().Send<Events::MouseMovedEvent>((float)xPos, (float)yPos);

		});

        GLFWInput* inputs = new GLFWInput;
        inputs->SetWindow(this);
        

#ifdef CHAOS_PLATFORM_WINDOWS
        typedef void(*SetInput)(Input*);
        HMODULE mod = GetModuleHandle(TEXT("ChaosEngine.dll"));
        CHAOS_CORE_ASSERT(mod, "ChaosEngine.dll is missing");
        SetInput setInput = (SetInput) ::GetProcAddress(mod, "SetInput");
        setInput(inputs);
#endif
    }

    void GLFWWindow::Close()
    {
        glfwDestroyWindow(m_window);
    }

    void GLFWWindow::SetVSync(bool p_enabled)
    {
        if (p_enabled)
        {
            glfwSwapInterval(1);
        }
        else
        {
            glfwSwapInterval(0);
        }

        m_data.vSync = p_enabled;
    }
}