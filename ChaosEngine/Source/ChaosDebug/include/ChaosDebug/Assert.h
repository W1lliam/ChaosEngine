#pragma once

#if !defined(CHAOS_SHIPPING)
    #define CHAOS_ENABLE_ASSERTS
#endif

#ifdef CHAOS_PLATFORM_WINDOWS
    #define CHAOS_DEBUG_BREAK __debugbreak();
#elif CHAOS_PLATFORM_UNIX
    #define CHAOS_DEBUG_BREAK __builtin_trap();
#endif

#ifdef CHAOS_ENABLE_ASSERTS
    #define CHAOS_ASSERT(x, ...)        { if(!(x)) { CHAOS_ERROR("Assertion Failed: {}\n File: {}, Line {}", __VA_ARGS__, __FILE__, __LINE__); CHAOS_DEBUG_BREAK } }
    #define CHAOS_CORE_ASSERT(x, ...)   { if(!(x)) { CHAOS_CORE_ERROR("Assertion Failed: {}\n File: {}, Line {}", __VA_ARGS__, __FILE__, __LINE__); CHAOS_DEBUG_BREAK } }
#else
    #define CHAOS_ASSERT(x, ...)
    #define CHAOS_CORE_ASSERT(x, ...)
#endif