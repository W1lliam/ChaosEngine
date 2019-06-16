#pragma once
#include "ChaosEngine/export.h"
#include <ChaosCore.h>

#pragma warning(push)
#pragma warning(disable: 4275)
#pragma warning(disable: 4251)

namespace Chaos
{

    class CHAOS_API Application : public Events::EventListener
    {
    public:
        Application();
        virtual ~Application() = default;

        virtual void Run();

        bool OnKeyPressed(const Events::KeyPressedEvent& e);
        bool OnKeyReleased(const Events::KeyReleasedEvent& e);
        bool OnWindowClose(const Events::WindowsCloseEvent& e);

	private:
        std::unique_ptr<Graphics::IWindow>  m_window        {nullptr};
        bool                                m_running       {true};
    };

    extern Application* CreateApplication();
}
#pragma warning(pop)
