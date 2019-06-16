#include "ChaosDebug/Log.h"

namespace Chaos::Debug
{
    Log& Log::Get()
    {
#ifdef CHAOS_PLATFORM_WINDOWS
        typedef Log& (*GetSingleton)();
        HMODULE mod = GetModuleHandle(TEXT("ChaosEngine.dll"));
        assert(mod && "ChaosEngine.dll is missing");
        GetSingleton getSingleton = (GetSingleton) ::GetProcAddress(mod,"GetLogger");
        return getSingleton();
#else
        static T instance;
        return instance;
#endif
    }

	Log::~Log()
	{
		spdlog::drop_all();
	}

	Log::Log()
    {
        spdlog::set_pattern("%^[%T] %-6n: %v%$");
        m_coreLogger = spdlog::stdout_color_mt("CHAOS");
        m_coreLogger->set_level(spdlog::level::trace);

        m_clientLogger = spdlog::stdout_color_mt("GAME");
        m_clientLogger->set_level(spdlog::level::trace);
    }
}