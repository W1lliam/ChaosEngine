#pragma once
#include "ChaosEngine/export.h"
#include "Globals.h"

namespace Chaos
{                
    class Input;

    CHAOS_API void Initialize();
    CHAOS_API void FixedUpdate(double p_fixedDeltaTime);
    CHAOS_API void Update(double p_deltaTime);
    CHAOS_API void Shutdown();

}