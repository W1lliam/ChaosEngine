#pragma once
#include <ChaosDebug.h>
#ifdef CHAOS_PLATFORM_WINDOWS
    #include <Windows.h> 
#endif
namespace Chaos
{
    template <class T>
    class Singleton
    {
    public:
        Singleton()     = default;
        ~Singleton()    = default;

        static T& Get()
        {
#ifdef CHAOS_PLATFORM_WINDOWS
            typedef T& (*GetSingleton)(); 
            HMODULE mod = GetModuleHandle(TEXT("ChaosEngine.dll"));
            CHAOS_CORE_ASSERT(mod, "ChaosEngine.dll is missing");
            std::string name = typeid(T).name();
            name = "Get" + name.substr(name.find_last_of(':') + 1);
            GetSingleton getSingleton = (GetSingleton) ::GetProcAddress(mod, name.c_str());
            return getSingleton();
#else
            static T instance;
            return instance;
#endif
        }
        static T* GetPointer()
        {
            return &Get();
        }

        Singleton(const Singleton<T>&) = delete;
        void operator=(const Singleton<T>&) = delete;

    };
}