#pragma once
#ifdef CHAOS_PLATFORM_WINDOWS
    #ifdef CHAOS_GRAPHICS_EXPORT
        #define __declspec(dllexport)
    #else
        #define __declspec(dllimport)
    #endif
#elif CHAOS_PLATFORM_UNIX
    #ifdef CHAOS_GRAPHICS_EXPORT
        #define __attribute__((visibility("default")))
    #else
        #define CHAOS_GRAPHICS_API
    #endif
#else
    #error Chaos Engine currently only supports Windows and Unix platforms
#endif