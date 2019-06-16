#pragma once
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/ECS/Component/ScriptedComponent.h"
#include "ChaosCore/Multithreading/Containers/ThreadSafeMap.h"
#include "ChaosCore/Multithreading/Containers/LockFree/LFQueue.h"
#include "ChaosCore/Utils/Singleton.h"
#include "ChaosCore/Utils/GUID.h"
#include "ChaosCore/Containers/RHHashTable.h"
#include <functional>


#ifdef foreach
#undef foreach
#endif

namespace Chaos
{	
	class ComponentManager : public Singleton<ComponentManager>
	{
	private:


		class IComponentContainer
		{
		public:
			virtual ~IComponentContainer()
			{}

			virtual void DestroyComponent(const ComponentID& p_componentID) = 0;
		}; //IComponentContainer

		template<typename T>
		class ComponentContainer : public IComponentContainer
		{
			using TComponentContainer = std::unordered_map<ComponentID, std::shared_ptr<T>>;

		private:
			TComponentContainer m_typedContainer;

		public:
			ComponentContainer() {}
			TComponentContainer& GetTComponentContainer() { return m_typedContainer; }
			void DestroyComponent(const ComponentID& p_componentID) override
			{
				m_typedContainer.erase(p_componentID);
			}
		}; //ComponentContainer<T>

		using IComponentContainerPtr = std::shared_ptr<IComponentContainer>;

	public:
        ComponentManager();
        ~ComponentManager();

        IComponent& LoadComponent(ReadStream& p_stream, const EntityID& p_entityID, const ComponentTypeID& p_typeID);

        template<typename ...Args>
        ScriptedComponent& AddScriptedComponent(const EntityID& p_entityID, const std::string& p_componentTypeName, Args&&... p_params)
        {
            ComponentID newID = ++m_newComponentID;

            std::shared_ptr<ScriptedComponent> newComponent = std::make_shared<ScriptedComponent>(p_entityID, newID, p_componentTypeName);
            newComponent->Init(std::forward<Args>(p_params)...);

            const ComponentTypeID CTID = newComponent->GetComponentTypeID();

            m_entityComponentTable[p_entityID][CTID] = newID;

            auto result = m_componentTable.try_emplace(newID, newComponent);
            CHAOS_ASSERT(result.second, "Error creating new Component !");

            auto result2 = GetComponentContainer<ScriptedComponent>()->GetTComponentContainer().try_emplace(newID, newComponent);
            CHAOS_ASSERT(result2.second, "Error inserting Component in typed containers !");

            return *newComponent.get();
        }



        template<typename T, typename ...Args>
		T& AddComponent(const EntityID& p_entityID, Args&&... p_params)
		{
			ComponentID newID = ++m_newComponentID;
			std::shared_ptr<T> newComponent = std::make_shared<T>(p_entityID, newID, std::forward<Args>(p_params)...);
			const ComponentTypeID CTID = T::COMPONENT_TYPE_ID;

            m_entityComponentTable[p_entityID][CTID] = newID;

            auto result = m_componentTable.try_emplace(newID, newComponent);
            CHAOS_ASSERT(result.second, "Error creating new Component !");

            auto result2 = GetComponentContainer<T>()->GetTComponentContainer().try_emplace(newID, newComponent);
            CHAOS_ASSERT(result2.second, "Error inserting Component in typed containers !");
            
			return *newComponent.get();
		}

		template<typename T>
		void RemoveComponent(const EntityID& p_entityID)
		{
			const ComponentTypeID CTID = T::COMPONENT_TYPE_ID;

			const ComponentID componentID = m_entityComponentTable[p_entityID][CTID];
			Events::EventHandler::Get().Send<Events::ComponentAboutToBeRemovedEvent>(*m_componentTable.at(componentID));
			m_componentPendingDeletion.push(m_componentTable.at(componentID));
			m_componentTable.erase(componentID);
		}

		void RemoveComponent(const EntityID& p_entityID, ComponentTypeID p_type);

		void RemoveAllComponents(const EntityID& p_entityId);
        void ProcessPendingDeletions();

        template<typename T>
        T* GetComponent(const EntityID p_entityID)
        {
            const ComponentTypeID CTID = T::COMPONENT_TYPE_ID;

            auto table1 = m_entityComponentTable.find(p_entityID);
            if (table1 != m_entityComponentTable.end())
            {
                auto table2 = table1->second.find(CTID);
                if (table2 != table1->second.end())
                {
                    auto component = m_componentTable.find(table2->second);
                    if (component != m_componentTable.end())
                    {
                        return dynamic_cast<T*>(component->second.get());
                    }
                }
            }
            return nullptr;
        }



        ScriptedComponent*                                  GetScriptedComponent(const EntityID& p_entityID, const std::string& p_componentTypeName);
        IComponent*                                         GetComponent(const EntityID p_entityID, const ComponentID p_componentID);
        IComponent*                                         GetComponent(const ComponentID p_componentID);
        std::unordered_map<ComponentTypeID, ComponentID>&   GetAllComponents(const EntityID p_entityID);
        void                                                MoveAllComponents(const EntityID p_entityID, const EntityID p_newEntityID);

		template<typename T>
        typename std::unordered_map<ComponentID, std::shared_ptr<T>>::iterator begin()
		{
			return GetComponentContainer<T>()->GetTComponentContainer().begin();
		}
		template<typename T>
        typename std::unordered_map<ComponentID, std::shared_ptr<T>>::iterator end()
		{
			return GetComponentContainer<T>()->GetTComponentContainer().end();
		}

    private:
        template<typename T>
        ComponentContainer<T>* GetComponentContainer()
        {
            ComponentTypeID CTID = T::COMPONENT_TYPE_ID;
            auto result = m_componentTypedTables.find(CTID);
            if (result != m_componentTypedTables.end())
                return reinterpret_cast<ComponentContainer<T>*>(result->second.get());

            m_componentTypedTables[CTID] = std::make_unique<ComponentContainer<T>>();
            result = m_componentTypedTables.find(CTID);
            CHAOS_CORE_ASSERT(result != m_componentTypedTables.end(), "Failed to create ComponentContainer<T>!");
            return reinterpret_cast<ComponentContainer<T>*>(result->second.get());
        }

        void RemoveComponent(ComponentPtr p_component);
        static void Expose();

    private:
        static constexpr const uint32_t MAX_COMPONENT_COUNT{ 512 };

        std::unordered_map<ComponentID, ComponentPtr>                                       m_componentTable;
        std::unordered_map<ComponentTypeID, IComponentContainerPtr>                         m_componentTypedTables;
        std::unordered_map<EntityID, std::unordered_map<ComponentTypeID, ComponentID>>      m_entityComponentTable;
        Containers::LockFree::LFQueue<ComponentPtr>                                         m_componentPendingDeletion;

        ComponentID                                                                         m_newComponentID = 0;
	};

}