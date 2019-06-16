#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "ChaosCore/ECS/Entity/AEntity.h"
#include "ChaosCore/Events/ECSEvents.h"
#include "ChaosCore/Events/EventListener.h"

namespace Chaos
{
	class EntityManager : public Singleton<EntityManager>, Events::EventListener
	{
	public:
        EntityManager();
		~EntityManager()    = default;
		
		template<typename T, typename ...Args>
		std::shared_ptr<T> CreateEntity(const EntityID& p_entityID, Args&&... p_params)
		{
			auto newEntity = std::make_shared<T>(p_entityID, std::forward<Args>(p_params)...);
			m_entityTable.insert_or_update(p_entityID, newEntity);
			CHAOS_CORE_ASSERT(m_entityTable.exists(p_entityID), "Error creating new Entity !");

			return newEntity;
		}

		void DestroyEntity(const EntityID& p_entityID);
		void RenameEntity(const EntityID& p_entityID, const EntityID& p_newEntityID);
        bool OnEntityRenamed(const Events::EntityRenamedEvent& p_event);

        EntityPtr GetEntity(const EntityID& p_entityID);


    private:
        Containers::ThreadSafeMap<EntityID, EntityPtr>				    m_entityTable;
	};
}