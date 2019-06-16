#pragma once
#include "ChaosGraphics/chaos_graphics_pch.h"
#include "ChaosCore.h"
#include "ChaosGraphics/Context/GLFWWindow.h"

namespace Chaos
{
    class GLFWInput : public Input
    {
    public:
        GLFWInput() = default;
        inline void SetWindow(Graphics::GLFWWindow* p_window) { m_window = p_window; };

    protected:
        virtual bool			IsKeyPressed(E_ChaosKeyCode keycode)            override;
        virtual bool			IsKeyReleased(E_ChaosKeyCode keycode)           override;
        virtual bool			IsMouseButtonPressed(E_ChaosMouseCode button)   override;
        virtual bool			IsMouseButtonReleased(E_ChaosMouseCode button)  override;
        virtual MousePos		GetMousePos()                                   override;
        virtual float			GetMouseX()                                     override;
        virtual float			GetMouseY()                                     override;

		virtual void			SetMousePosition(double p_x, double p_y)	    override;
		virtual void			HideMouse(bool p_hide)					        override;

    protected:
        Graphics::GLFWWindow*   m_window    {nullptr};
    };
}