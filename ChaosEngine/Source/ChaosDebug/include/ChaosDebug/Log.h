#pragma once
#include "ChaosDebug/chaos_debug_pch.h"

#pragma warning(push)
#pragma warning(disable: 4251)

namespace Chaos
{
    class Loader;
}


namespace Chaos::Debug
{
    class Log
    {
    public:
        static Log& Get();

        inline std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_coreLogger; }
        inline std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_clientLogger; }

		~Log();
    private:
        static void Expose();

        friend class ::Chaos::Loader;
        Log();
        Log(const Log& p_other) = delete;
        Log operator=(const Log& p_other) = delete;
        std::shared_ptr<spdlog::logger> m_coreLogger;
        std::shared_ptr<spdlog::logger> m_clientLogger;
    };
}

#if !defined(CHAOS_SHIPPING)
// Core Logging Macros
#define CHAOS_CORE_TRACE(...)   ::Chaos::Debug::Log::Get().GetCoreLogger()->trace(__VA_ARGS__)
#define CHAOS_CORE_INFO(...)    ::Chaos::Debug::Log::Get().GetCoreLogger()->info(__VA_ARGS__)
#define CHAOS_CORE_WARN(...)    ::Chaos::Debug::Log::Get().GetCoreLogger()->warn(__VA_ARGS__)


// Client Logging Macros
#define CHAOS_TRACE(...)        ::Chaos::Debug::Log::Get().GetClientLogger()->trace(__VA_ARGS__)
#define CHAOS_INFO(...)         ::Chaos::Debug::Log::Get().GetClientLogger()->info(__VA_ARGS__)
#define CHAOS_WARN(...)         ::Chaos::Debug::Log::Get().GetClientLogger()->warn(__VA_ARGS__)

#else
// Core Logging Macros
#define CHAOS_CORE_TRACE(...)
#define CHAOS_CORE_INFO(...)
#define CHAOS_CORE_WARN(...)

// Client Logging Macros
#define CHAOS_TRACE(...)
#define CHAOS_INFO(...)
#define CHAOS_WARN(...)
#endif

#define CHAOS_CORE_ERROR(...)   ::Chaos::Debug::Log::Get().GetCoreLogger()->error(__VA_ARGS__)
#define CHAOS_CORE_FATAL(...)   ::Chaos::Debug::Log::Get().GetCoreLogger()->critical(__VA_ARGS__)
#define CHAOS_ERROR(...)        ::Chaos::Debug::Log::Get().GetClientLogger()->error(__VA_ARGS__)
#define CHAOS_FATAL(...)        ::Chaos::Debug::Log::Get().GetClientLogger()->critical(__VA_ARGS__)

#pragma warning(pop)
