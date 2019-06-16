#pragma once
#include <ChaosCore.h>

namespace Chaos::Graphics
{
    class GLFWWindow : public IWindow
    {

    public:
        GLFWWindow(const WindowConfiguration& p_configuration);
        ~GLFWWindow();

        virtual void OnUpdate() override;

        virtual void SetVSync(bool p_enabled) override;

        inline virtual void*    GetNativeWindow()					const override { return m_window; };
        inline virtual uint16_t GetWidth()							const override { return m_data.width; }
        inline virtual uint16_t GetHeight()							const override { return m_data.height; }
        inline virtual void		SetWidth(uint16_t p_width)				  override { m_data.width = p_width; }
        inline virtual void		SetHeight(uint16_t p_height)			  override { m_data.height = p_height; }
        inline virtual bool     IsVSyncEnabled()					const override { return m_data.vSync; }

    private:
        void Init(const WindowConfiguration& p_configuration);
        void Close();

    private:
        struct WindowData
        {
            std::string title;
            uint16_t    width;
            uint16_t    height;
            bool        vSync;
        };

        WindowData  m_data{};
        GLFWwindow* m_window{ nullptr };

    };
}