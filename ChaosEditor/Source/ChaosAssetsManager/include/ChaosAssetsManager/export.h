#pragma once
#ifdef CHAOS_PLATFORM_WINDOWS
    #ifdef CHAOS_ASSETS_MANAGER_EXPORT
        #define CHAOS_ASSETS_MANAGER_API __declspec(dllexport)
    #else
        #define CHAOS_ASSETS_MANAGER_API __declspec(dllimport)
    #endif
#elif CHAOS_PLATFORM_UNIX
    #ifdef CHAOS_ASSETS_MANAGER_EXPORT
        #define CHAOS_ASSETS_MANAGER_API __attribute__((visibility("default")))
    #else
        #define CHAOS_ASSETS_MANAGER_API
    #endif
#else
    #error Chaos Editor currently only supports Windows and Unix platforms
#endif