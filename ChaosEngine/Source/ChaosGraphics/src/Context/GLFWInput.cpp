#include "ChaosGraphics/Context/GLFWInput.h"
#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosGraphics/Context/GLFWWindow.h"

namespace Chaos
{
    bool GLFWInput::IsKeyPressed(E_ChaosKeyCode keycode)
    {
        auto	state	= glfwGetKey(static_cast<GLFWwindow*>(m_window->GetNativeWindow()), (int)keycode);
        return	state	== GLFW_PRESS;
    }

	bool GLFWInput::IsKeyReleased(E_ChaosKeyCode keycode)
	{
		auto	state	= glfwGetKey(static_cast<GLFWwindow*>(m_window->GetNativeWindow()), (int)keycode);
		return	state	== GLFW_RELEASE;
	}

    bool GLFWInput::IsMouseButtonPressed(E_ChaosMouseCode button)
    {
        auto	state	= glfwGetMouseButton(static_cast<GLFWwindow*>(m_window->GetNativeWindow()), (int)button);
        return	state	== GLFW_PRESS;
    }

	bool GLFWInput::IsMouseButtonReleased(E_ChaosMouseCode button)
	{
		auto	state = glfwGetMouseButton(static_cast<GLFWwindow*>(m_window->GetNativeWindow()), (int)button);
		return	state == GLFW_RELEASE;
	}

    Input::MousePos GLFWInput::GetMousePos()
    {
        double xpos, ypos;
        glfwGetCursorPos(static_cast<GLFWwindow*>(m_window->GetNativeWindow()), &xpos, &ypos);
        return { static_cast<float>(xpos), static_cast<float>(ypos) };
    }

    float GLFWInput::GetMouseX()
    {
        auto[x,y] = GetMousePos();
        return x;
    }

    float GLFWInput::GetMouseY()
    {
        auto[x, y] = GetMousePos();
        return y;
    }

	void GLFWInput::SetMousePosition(double p_x, double p_y)
	{
		glfwSetCursorPos(static_cast<GLFWwindow*>(m_window->GetNativeWindow()), p_x, p_y);
	}

	void GLFWInput::HideMouse(bool p_hide)
	{
		if(p_hide)
			glfwSetInputMode(static_cast<GLFWwindow*>(m_window->GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(static_cast<GLFWwindow*>(m_window->GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}