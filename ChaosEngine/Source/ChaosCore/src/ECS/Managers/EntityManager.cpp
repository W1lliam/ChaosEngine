#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/Events/EventHandler.h"

namespace Chaos
{
    EntityManager::EntityManager() 
    { 
        AEntity::Expose(); 
        RegisterEventCallback<Events::EntityRenamedEvent>(std::bind(&EntityManager::OnEntityRenamed, this, std::placeholders::_1));
    }

    void EntityManager::DestroyEntity(const EntityID& p_entityID)
    {
        if (!m_entityTable.exists(p_entityID))
        {
            CHAOS_CORE_WARN("Entity {0} not found", p_entityID);
            return;
        }

        ComponentManager::Get().RemoveAllComponents(p_entityID);
        m_entityTable.erase(p_entityID);
    }

    void EntityManager::RenameEntity(const EntityID & p_entityID, const EntityID & p_newEntityID)
    {
        Events::EventHandler::Get().Send<Events::EntityRenamedEvent>(p_entityID, p_newEntityID);
    }

    bool EntityManager::OnEntityRenamed(const Events::EntityRenamedEvent& p_event)
    {
        if (m_entityTable.exists(p_event.GetEntityID()))
        {
            auto entity = m_entityTable.at(p_event.GetEntityID());
            m_entityTable.erase(p_event.GetEntityID());
            m_entityTable.insert_or_update(p_event.GetNewEntityID(), entity);
            ComponentManager::Get().MoveAllComponents(p_event.GetEntityID(), p_event.GetNewEntityID());
            entity->m_entityID = p_event.GetEntityID();
        }
        return false;
    }

    EntityPtr EntityManager::GetEntity(const EntityID& p_entityID)
    {
        if (m_entityTable.exists(p_entityID))
        {
            return m_entityTable.at(p_entityID);
        }
        return nullptr;
    }
}