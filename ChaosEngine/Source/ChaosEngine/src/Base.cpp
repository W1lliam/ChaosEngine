#include "ChaosEngine/Base.h"
#include "ChaosDebug.h"
#include "ChaosCore.h"
#include "ChaosEngine/Loader.h"


namespace Chaos 
{
    void Initialize()
    {
        if(!::Chaos::Loader::GetPointer())
            new ::Chaos::Loader();
        CHAOS_CORE_TRACE("Chaos Engine {}", CHAOS_ENGINE_VERSION);
    }

    void FixedUpdate(double p_fixedDeltaTime)
	{
        Loader::Get().GetSystemManager().FixedUpdate(p_fixedDeltaTime);
	}

    void Update(double p_deltaTime)
	{
        Loader::Get().GetSystemManager().Update(p_deltaTime);
	}

    void Shutdown()
    {
        CHAOS_CORE_TRACE("Shutting down...");
        if (Loader::GetPointer())
            delete Loader::GetPointer();
    }
}