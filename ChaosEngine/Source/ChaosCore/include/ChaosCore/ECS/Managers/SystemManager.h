#pragma once

#include "ChaosCore/Utils/Singleton.h"
#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/ECS/System/ScriptedSystem.h"
#include "ChaosCore/Multithreading/Containers/ThreadSafeMap.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace Chaos
{
	class SystemManager : public Singleton<SystemManager>
	{
	public:
        SystemManager() = default;
		~SystemManager();

        void Start();
        void Stop();

        void FixedUpdate(double p_fixedDeltaTime_ms);
        void Update(double p_deltaTime_ms);

        ScriptedSystem& RegisterScriptedSystem(ssq::VM& p_vm);

       
        template<class T, class... ARGS>
        T* AddSystem(ARGS&&... systemArgs)
		{
			const SystemTypeID STID = T::SYSTEM_TYPE_ID;

			// avoid multiple registrations of the same system
            if(m_systemTable.exists(STID))
				return reinterpret_cast<T*>(m_systemTable.at(STID).get());

			auto system = std::make_shared<T>(std::forward<ARGS>(systemArgs)...);
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
            std::unique_lock<std::shared_mutex> lock(m_mutex);
			m_systemWorkOrder.push_back(system);

			return system.get();
		}

		template<class System_, class Dependency_>
		void AddSystemDependency(System_ target, Dependency_ dependency)
		{
			const TypeID TARGET_ID = target->GetSystemTypeID();
			const TypeID DEPEND_ID = dependency->GetSystemTypeID();

			if (!m_systemDependencyTable.at(TARGET_ID)->exists(DEPEND_ID) || m_systemDependencyTable.at(TARGET_ID)->at(DEPEND_ID) != true)
			{
				m_systemDependencyTable.at(TARGET_ID)->insert_or_update(DEPEND_ID, true);
				CHAOS_CORE_INFO("Added {0} as dependency to {1}", dependency->GetSystemTypeName(), target->GetSystemTypeName());
			}
		}

		template<class Target_, class Dependency_, class... Dependencies>
		void AddSystemDependency(Target_ target, Dependency_ dependency, Dependencies&&... dependencies)
		{
			const TypeID TARGET_ID = target->GetSystemTypeID();
			const TypeID DEPEND_ID = dependency->GetSystemTypeID();
            
			if (!m_systemDependencyTable.at(TARGET_ID)->exists(DEPEND_ID) || m_systemDependencyTable.at(TARGET_ID)->at(DEPEND_ID) != true)
			{
				m_systemDependencyTable.at(TARGET_ID)->insert_or_update(DEPEND_ID, true);
				CHAOS_CORE_TRACE("Added {0} as dependency to {0}", dependency->GetSystemTypeName(), target->GetSystemTypeName());
			}
			AddSystemDependency(target, std::forward<Dependencies>(dependencies)...);
		}

		void UpdateSystemWorkOrder();

        template<class T>
        inline std::shared_ptr<T> GetSystem()
        {
            const SystemTypeID STID = T::SYSTEM_TYPE_ID;
            if (m_systemTable.exists(STID))
            {
                return std::reinterpret_pointer_cast<T>(m_systemTable.at(STID));
            }
            return nullptr;
        }

        inline SystemPtr GetSystem(const SystemTypeID& p_systemTypeID)
        {
            if (m_systemTable.exists(p_systemTypeID))
            {
                return m_systemTable.at(p_systemTypeID);
            }
            CHAOS_CORE_WARN("System {0} not found", p_systemTypeID);
            return nullptr;
        }

		template<class T>
		void EnableSystem()
		{
			const SystemTypeID STID = T::SYSTEM_TYPE_ID;
            if (m_systemTable.exists(STID))
            {
                m_systemTable.at(STID)->Enable();
                return;
            }
            CHAOS_CORE_WARN("Trying to enable system {0}, but system is not registered yet.", STID);
		}

        void DisableAllSystems();

		template<class T>
        void DisableSystem()
        {
            const SystemTypeID STID = T::SYSTEM_TYPE_ID;
            if (m_systemTable.exists(STID))
            {
                m_systemTable.at(STID)->Disable();
                return;
            }
            CHAOS_CORE_WARN("Trying to disable system {0}, but system is not registered yet.", STID);
        }

        void DisableSystem(const SystemTypeID& p_systemTypeID);

		template<class T>
		void SetSystemUpdateInterval(double interval_ms)
		{
			const SystemTypeID STID = T::SYSTEM_TYPE_ID;
            if (m_systemTable.exists(STID))
            {
                m_systemTable.at(STID)->m_updateInterval = interval_ms;
                return;
            }
            CHAOS_CORE_WARN("Trying to change system {0} update interval, but system is not registered yet.", STID);
		}

        System& LoadSystem(ReadStream& p_stream, const SystemTypeID& p_typeID, const std::string& p_systemName = "");

        inline bool IsInPlayMode() const { return m_isInPlayMode; }

        template<typename Function>
        void foreach(Function p_function)
        {
            for (SystemPtr system : m_systemWorkOrder)
            {
                p_function(system.get());
            }
        }


    private:
        Containers::ThreadSafeMap<SystemTypeID, SystemPtr> m_systemTable;

        // System Dependency Table
        //    ---   |	Dependency SystemID |
        // SystemID |   0   |   1   |   2   |
        //     0    | true  | false | false |
        //     1    | false | false | false |
        //     2    | false | true  | false |
        Containers::ThreadSafeMap<SystemTypeID, Containers::ThreadSafeMap<SystemTypeID, bool>*> m_systemDependencyTable;
        std::vector<SystemPtr>      m_systemWorkOrder;
        std::shared_mutex           m_mutex;
        bool                        m_isInPlayMode = false;
	};
}

#pragma warning(pop)