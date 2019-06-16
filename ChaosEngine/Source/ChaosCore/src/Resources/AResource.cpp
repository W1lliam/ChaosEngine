#include "ChaosCore/Resources/AResource.h"
#include <ChaosDebug.h>

namespace Chaos
{
    AResource::AResource(AResourceManager* p_creator, const StringID& p_name, const StringID& p_group) : 
        m_creator{ p_creator },
        m_name{ p_name },
        m_group{ p_group }
    {}

    void AResource::Prepare()
    {
        E_ResourceState unprepared = E_ResourceState::UNPREPARED;
        if (!m_state.compare_exchange_strong(unprepared, E_ResourceState::PREPARING))
        {
            CHAOS_CORE_WARN("Resource is already prepared");
            return;
        }

        try
        {
            PrepareImpl();
        }
        catch (...)
        {
            m_state.store(E_ResourceState::UNPREPARED);
            throw;
        }
        m_state.store(E_ResourceState::PREPARED);
    }

    void AResource::Load()
    {
        E_ResourceState prevState = m_state.load();
        if (prevState == E_ResourceState::PREPARING)
        {
            while (m_state.load() == E_ResourceState::PREPARING)
            {}
            prevState = m_state.load();
        }

        if (prevState != E_ResourceState::PREPARED && prevState != E_ResourceState::UNPREPARED)
        {
            CHAOS_CORE_WARN("Resource is not prepared or already loaded");
            return;
        }

        while (!m_state.compare_exchange_weak(prevState, E_ResourceState::LOADING)) {}

        try
        {
            if (prevState == E_ResourceState::UNPREPARED)
            {
                PrepareImpl();
            }
            PreLoadImpl();
            LoadImpl();
            PostLoadImpl();
        }
        catch (...)
        {
            m_state.store(E_ResourceState::UNPREPARED);
            throw;
        }
        m_state.store(E_ResourceState::LOADED);
    }
    void AResource::Unload()
    {
        // Early-out without lock (mitigate perf cost of ensuring unloaded)
        E_ResourceState previousState = m_state.load();
        if (previousState != E_ResourceState::LOADED && previousState != E_ResourceState::PREPARED) return;


        if (!m_state.compare_exchange_strong(previousState, E_ResourceState::UNLOADING)) 
            return;

        if (previousState == E_ResourceState::PREPARED)
        {
            UnprepareImpl();
        }
        else 
        {
            PreUnloadImpl();
            UnloadImpl();
            PostUnloadImpl();
        }

        m_state.store(E_ResourceState::UNPREPARED);
    }
    void AResource::Reload()
    {
        if (m_state.load() == E_ResourceState::LOADED)
        {
            CHAOS_CORE_TRACE("Reloading Resource {0}", m_name);
            Unload();
            Load();
        }
    }
}