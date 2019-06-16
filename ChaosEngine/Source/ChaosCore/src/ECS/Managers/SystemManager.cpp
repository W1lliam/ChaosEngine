#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/ECS/Types.h"
#include "ChaosCore/Utils/algorithm.h"
#include "ChaosCore/Events/EventHandler.h"
#include "ChaosCore/Systems/CameraSystem.h"
#include "ChaosCore/Systems/LightSystem.h"
#include "ChaosCore/Systems/PhysicsSystem.h"
#include "ChaosCore/Systems/RenderSystem.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Systems/AudioSystem.h"
#include "ChaosCore/Systems/AnimationSystem.h"
#include "ChaosCore/ECS/System/ScriptedSystem.h"

namespace Chaos
{
    SystemManager::~SystemManager()
    {
        m_systemDependencyTable.for_each([&](std::pair<SystemTypeID, Containers::ThreadSafeMap<SystemTypeID, bool>*>& p_pair)
        {
            delete p_pair.second;
            p_pair.second = nullptr;
        });
    }

    void SystemManager::Start()
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        m_isInPlayMode = true;
        Events::EventHandler::Get().DispatchEvents();
        ComponentManager::Get().ProcessPendingDeletions();
        for (SystemPtr system : m_systemWorkOrder)
        {
            // increase interval since last update
            system->m_timeSinceLastUpdate = 0.0f;

            if (system->m_enabled && m_isInPlayMode && (system->GetExecutionMode() & E_SystemExecutionModeFlags::GAME))
            {
                system->Start();
            }
        }
        Events::EventHandler::Get().DispatchEvents();
        ComponentManager::Get().ProcessPendingDeletions();
    }

    void SystemManager::Stop()
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        m_isInPlayMode = false;
        Events::EventHandler::Get().DispatchEvents();
        ComponentManager::Get().ProcessPendingDeletions();
        for (SystemPtr system : m_systemWorkOrder)
        {
            // increase interval since last update
            system->m_timeSinceLastUpdate = 0.0f;

            if (system->m_enabled && system->GetExecutionMode() & E_SystemExecutionModeFlags::GAME)
            {
                system->Stop();
            }
        }
        Events::EventHandler::Get().DispatchEvents();
        ComponentManager::Get().ProcessPendingDeletions();
    }

    void SystemManager::FixedUpdate(const double p_fixedDeltaTime_ms)
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        for (SystemPtr system : m_systemWorkOrder)
        {
            // increase interval since last update
            system->m_timeSinceLastUpdate = p_fixedDeltaTime_ms;

            if (system->m_enabled && system->m_needsUpdate == true &&
                (m_isInPlayMode && system->GetExecutionMode() & E_SystemExecutionModeFlags::GAME) ||
                (!m_isInPlayMode && system->GetExecutionMode() & E_SystemExecutionModeFlags::EDITOR))
            {
                system->FixedUpdate(p_fixedDeltaTime_ms);
            }
        }
        Events::EventHandler::Get().DispatchEvents();
        ComponentManager::Get().ProcessPendingDeletions();
    }

    void SystemManager::Update(const double p_deltaTime_ms)
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        for (SystemPtr system : m_systemWorkOrder)
        {
            // increase interval since last update
            system->m_timeSinceLastUpdate = p_deltaTime_ms;

            // check systems update state
            system->m_needsUpdate = (system->m_updateInterval < 0.0f) || ((system->m_updateInterval > 0.0f) && (system->m_timeSinceLastUpdate > system->m_updateInterval));

            if (system->m_enabled && system->m_needsUpdate == true &&
                ((m_isInPlayMode && system->GetExecutionMode() & E_SystemExecutionModeFlags::GAME) ||
                (!m_isInPlayMode && system->GetExecutionMode() & E_SystemExecutionModeFlags::EDITOR)))
            {
                system->PreUpdate(p_deltaTime_ms);
            }
        }
        Events::EventHandler::Get().DispatchEvents();
        ComponentManager::Get().ProcessPendingDeletions();

        for (SystemPtr system : m_systemWorkOrder)
        {
            if (system->m_enabled && system->m_needsUpdate == true && 
                ((m_isInPlayMode && system->GetExecutionMode() & E_SystemExecutionModeFlags::GAME) ||
                (!m_isInPlayMode && system->GetExecutionMode() & E_SystemExecutionModeFlags::EDITOR)))
            {
                system->Update(p_deltaTime_ms);
                system->m_timeSinceLastUpdate = 0.0f;
            }
        }
        Events::EventHandler::Get().DispatchEvents();
        ComponentManager::Get().ProcessPendingDeletions();

        for (SystemPtr system : m_systemWorkOrder)
        {
            if (system->m_enabled && system->m_needsUpdate == true && 
                ((m_isInPlayMode && system->GetExecutionMode() & E_SystemExecutionModeFlags::GAME) ||
                (!m_isInPlayMode && system->GetExecutionMode() & E_SystemExecutionModeFlags::EDITOR)))
            {
                system->PostUpdate(p_deltaTime_ms);
            }
        }
        Events::EventHandler::Get().DispatchEvents();
        ComponentManager::Get().ProcessPendingDeletions();
    }

    ScriptedSystem& SystemManager::RegisterScriptedSystem(ssq::VM& p_vm)
    {

        // avoid multiple registrations of the same system
        bool alreadyRegistered = false;
        std::shared_ptr<ScriptedSystem> system = std::make_shared<ScriptedSystem>(p_vm);
        const SystemTypeID STID = system->GetSystemTypeID();
        if (m_systemTable.exists(STID))
        {
            CHAOS_CORE_WARN("ScriptedSystem already registered");
            alreadyRegistered = true;
        }

        system->m_systemManager = this;
        m_systemTable.insert_or_update(STID, system);

        // resize dependency table
        Containers::ThreadSafeMap<SystemTypeID, bool>* systemDependency;
        if (m_systemDependencyTable.exists(STID))
        {
            systemDependency = m_systemDependencyTable.at(STID);
        }
        else
        {
            systemDependency = new Containers::ThreadSafeMap<SystemTypeID, bool>;
        }
        m_systemTable.for_each([&systemDependency](auto& p_pair)
        {
            systemDependency->insert_or_update(p_pair.first, false);
        });
        m_systemDependencyTable.insert_or_update(STID, systemDependency);



        // add to work list
        //Not thread-safe!! Use a queue to add to vector
        if (!alreadyRegistered)
        {
            m_systemWorkOrder.push_back(system);
        }
        else
        {
            auto foundSystem = std::find_if(m_systemWorkOrder.begin(), m_systemWorkOrder.end(), [&system](const SystemPtr& p_system) { return p_system->GetSystemTypeID() == system->GetSystemTypeID();  });
            CHAOS_ASSERT(foundSystem != m_systemWorkOrder.end(), "Registered System not found in work order");
            *foundSystem = system;
        }

        return *system;
    }


    void SystemManager::UpdateSystemWorkOrder()
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        const size_t systemsCount = m_systemWorkOrder.size();

        // create index array
        std::vector<SystemTypeID> indices(systemsCount);
        int i = 0;
        m_systemDependencyTable.for_each([&](auto& p_pair)
        {
            CHAOS_ASSERT(i < systemsCount, "Too many systems in dependency table!");
            indices[i] = p_pair.first;
            ++i;
        });
        CHAOS_ASSERT(i == systemsCount, "Wrong system count in dependency table!");

        // determine vertex-groups
        std::vector<std::vector<SystemTypeID>>	vertexGroups;
        std::vector<SystemPriority>				groupPriority;

        while (!indices.empty())
        {
            SystemTypeID index = indices.back();
            indices.pop_back();

            if (index == -1)
                continue;

            std::vector<SystemTypeID> group;
            std::vector<SystemTypeID> member;

            SystemPriority currentGroupPriority = System::LOWEST_SYSTEM_PRIORITY;
            member.push_back(index);

            while (!member.empty())
            {
                index = member.back();
                member.pop_back();

                if (index == INVALID_TYPEID)
                    continue;

                for (int i = 0; i < static_cast<int>(indices.size()); ++i)
                {
                    if (indices[i] != INVALID_TYPEID && !m_systemDependencyTable.at(indices[i])->exists(index))
                        m_systemDependencyTable.at(indices[i])->insert_or_update(index, false);
                    if (indices[i] != INVALID_TYPEID && !m_systemDependencyTable.at(index)->exists(indices[i]))
                        m_systemDependencyTable.at(index)->insert_or_update(indices[i], false);

                    if (indices[i] != INVALID_TYPEID && (m_systemDependencyTable.at(indices[i])->at(index) == true || m_systemDependencyTable.at(index)->at(indices[i]) == true))
                    {
                        member.push_back(indices[i]);
                        indices[i] = INVALID_TYPEID;
                    }
                }

                group.push_back(index);

                currentGroupPriority = std::max((m_systemTable.exists(index) ? m_systemTable.at(index)->m_priority : System::NORMAL_SYSTEM_PRIORITY), currentGroupPriority);
            }

            vertexGroups.push_back(group);
            groupPriority.push_back(currentGroupPriority);
        }

        const size_t vertexGroupsCount = vertexGroups.size();

        // do a topological sort on groups w.r.t. to groups priority!
        std::unordered_map<SystemTypeID, int> vertex_states(systemsCount);
        i = 0;
        m_systemDependencyTable.for_each([&](auto& p_pair)
        {
            CHAOS_ASSERT(i <= systemsCount, "Too many systems in dependency table!");
            vertex_states[p_pair.first] = 0;
            ++i;
        });
        std::multimap<SystemPriority, std::vector<SystemTypeID>> sortedVertexGroups;


        for (size_t i = 0; i < vertexGroupsCount; ++i)
        {
            std::vector<SystemTypeID> g = vertexGroups[i];

            std::vector<SystemTypeID> order;

            for (size_t j = 0; j < g.size(); ++j)
            {
                if (vertex_states[g[j]] == 0)
                    Utils::depthFirstSearch<SystemTypeID>(g[j], vertex_states, m_systemDependencyTable, order);
            }

            std::reverse(order.begin(), order.end());

            sortedVertexGroups.insert(std::pair<SystemPriority, std::vector<SystemTypeID>>(std::numeric_limits<SystemPriority>::max() - groupPriority[i], order));
        }

        // re-build system work order
        m_systemWorkOrder.clear();
        for (auto group : sortedVertexGroups)
        {
            for (auto m : group.second)
            {
                if (m_systemTable.exists(m))
                {
                    m_systemWorkOrder.push_back(m_systemTable.at(m));
                }
            }
        }
    }

    void SystemManager::DisableAllSystems()
    {
        for (SystemPtr system : m_systemWorkOrder)
        {
            system->Disable();
        }
    }

    void SystemManager::DisableSystem(const SystemTypeID & p_systemTypeID)
    {
        if (m_systemTable.exists(p_systemTypeID))
        {
            m_systemTable.at(p_systemTypeID)->Disable();
            return;
        }
        CHAOS_CORE_WARN("Trying to disable system {0}, but system is not registered yet.", p_systemTypeID);
    }

    System& SystemManager::LoadSystem(ReadStream& p_stream, const SystemTypeID& p_typeID, const std::string& p_systemName)
    {
        if (p_typeID == CameraSystem::SYSTEM_TYPE_ID)
        {
            return CameraSystem::Load(p_stream);
        }
        else if (p_typeID == LightSystem::SYSTEM_TYPE_ID)
        {
            return LightSystem::Load(p_stream);
        }
        else if (p_typeID == PhysicsSystem::SYSTEM_TYPE_ID)
        {
            return PhysicsSystem::Load(p_stream);
        }
        else if (p_typeID == RenderSystem::SYSTEM_TYPE_ID)
        {
            return RenderSystem::Load(p_stream);
        }
        else if (p_typeID == SceneSystem::SYSTEM_TYPE_ID)
        {
            return SceneSystem::Load(p_stream);
        }
        else if (p_typeID == AudioSystem::SYSTEM_TYPE_ID)
        {
            return AudioSystem::Load(p_stream);
        }
        else if (p_typeID == AnimationSystem::SYSTEM_TYPE_ID)
        {
            return AnimationSystem::Load(p_stream);
        }
        else
        {
            return ScriptedSystem::Load(p_stream, p_typeID, p_systemName);
        }
    }
}