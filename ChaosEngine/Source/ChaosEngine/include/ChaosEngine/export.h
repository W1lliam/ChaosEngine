#pragma once
#ifdef CHAOS_PLATFORM_WINDOWS
    #ifdef CHAOS_ENGINE_EXPORT
        #define CHAOS_API __declspec(dllexport)
    #else
        #define CHAOS_API __declspec(dllimport)
    #endif
#elif CHAOS_PLATFORM_UNIX
    #ifdef CHAOS_ENGINE_EXPORT
        #define CHAOS_API __attribute__((visibility("default")))
    #else
        #define CHAOS_API
    #endif
#else
    #error Chaos Engine currently only supports Windows and Unix platforms
#endif

#ifndef CHAOS_API
    #define CHAOS_API
#endif