#pragma once
#include "ChaosEngine.h"
#include "ChaosGraphics/Context/GLFWInput.h"

#ifdef CHAOS_PLATFORM_WINDOWS
int main()
{
    Chaos::Initialize();
    Chaos::Application* app = Chaos::CreateApplication();
    app->Run();
    delete app;
    Chaos::Shutdown();
    return 0;
}
#endif // CHAOS_PLATFORM_WINDOWS


#ifdef CHAOS_PLATFORM_UNIX
int main()
{
    Chaos::Initialize();
    Chaos::Application* app = Chaos::CreateApplication();
    app->Run();
    delete app;
    Chaos::Shutdown();
    return 0;
}
#endif // CHAOS_PLATFORM_UNIX
