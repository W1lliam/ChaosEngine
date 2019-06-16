#pragma once
#include "ChaosCore/Events/AEvent.h"
#include "ChaosCore/Events/EventListener.h"

#pragma warning(push)
#pragma warning(disable:4275)

namespace Chaos::Graphics
{
    struct WindowConfiguration
    {
        std::string title;
        uint16_t width;
        uint16_t height;

        WindowConfiguration(const std::string& p_title = "ChaosEngine",
            uint16_t p_width = 1280,
            uint16_t p_height = 720)
            : title(p_title), width(p_width), height(p_height)
        {}
    };

    class IWindow
    {
    public:
		virtual ~IWindow() = default;

        virtual void        OnUpdate()							    = 0;
        virtual void        SetVSync(bool p_enabled)			    = 0;
        
        virtual void*       GetNativeWindow()				const   = 0;
        virtual uint16_t    GetWidth()						const   = 0;
        virtual uint16_t    GetHeight()						const   = 0;
        virtual void		SetWidth(uint16_t p_width)				= 0;
        virtual void	    SetHeight(uint16_t p_height)			= 0;
        virtual bool        IsVSyncEnabled()				const   = 0;

        static IWindow*     Create(const WindowConfiguration& p_configuration = WindowConfiguration{});
    };
}

#pragma warning(pop)
