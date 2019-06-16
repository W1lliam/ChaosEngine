#pragma once
#include "ChaosCore/Events/AEvent.h"
#include "KeyCodes.h"

namespace Chaos
{
    class IComponent;
    using ComponentPtr = std::shared_ptr<IComponent>;
}

namespace Chaos::Events
{
    class AECSEvent : public AEvent
    {
    public:

        EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_ECS)

    protected:
        AECSEvent(E_EventType eType) : AEvent(eType) {}
    };

    class ComponentCreatedEvent : public AECSEvent
    {
    public:
        ComponentCreatedEvent(IComponent& p_component) : AECSEvent(E_EventType::COMPONENT_CREATED), m_component(p_component) {}
        inline const IComponent& GetComponent() const { return m_component; }

        inline void Print() const override
        {
            CHAOS_CORE_INFO("ComponentCreatedEvent:\n\tComponentID {1}", m_component.GetComponentID());
        }
        EVENT_CLASS_TYPE(COMPONENT_CREATED)

    private:
        IComponent& m_component;
    };

    class ComponentAboutToBeRemovedEvent : public AECSEvent
    {
    public:
        ComponentAboutToBeRemovedEvent(IComponent& p_component) : AECSEvent(E_EventType::COMPONENT_ABOUT_TO_BE_REMOVED), m_component(p_component) {}
        inline const IComponent& GetComponent() const { return m_component; }

        inline void Print() const override
        {
            CHAOS_CORE_INFO("ComponentAboutToBeRemovedEvent:\n\tComponentID {1}", m_component.GetComponentID());
        }
        EVENT_CLASS_TYPE(COMPONENT_ABOUT_TO_BE_REMOVED)

    private:
        IComponent& m_component;
    };

    class ComponentRemovedEvent : public AECSEvent
    {
    public:
        ComponentRemovedEvent(ComponentTypeID p_componentTypeID, ComponentID p_componentID) : 
            AECSEvent(E_EventType::COMPONENT_REMOVED), 
            m_componentTypeID(p_componentTypeID),
            m_componentID(p_componentID) 
        {}
        inline ComponentTypeID  GetComponentTypeID()    const { return m_componentTypeID; }
        inline ComponentID      GetComponentID()        const { return m_componentID; }

        inline void Print() const override
        {
            CHAOS_CORE_INFO("ComponentRemovedEvent:\n\tComponentType {0} - ComponentID {1}", m_componentTypeID, m_componentID);
        }
        EVENT_CLASS_TYPE(COMPONENT_REMOVED)

    private:
        ComponentTypeID m_componentTypeID;
        ComponentID     m_componentID;
    };

    class EntityRenamedEvent : public AECSEvent
    {
    public:
        EntityRenamedEvent(EntityID p_entityID, EntityID p_newEntityID) :
            AECSEvent(E_EventType::ENTITY_RENAMED),
            m_entityID(p_entityID),
            m_newEntityID(p_newEntityID)
        {}
        inline EntityID     GetEntityID()       const { return m_entityID; }
        inline EntityID     GetNewEntityID()    const { return m_newEntityID; }

        inline void Print() const override
        {
            CHAOS_CORE_INFO("EntityRenamedEvent:\n\tPrevious EntityID {0} - New EntityID {1}", m_entityID, m_newEntityID);
        }
        EVENT_CLASS_TYPE(ENTITY_RENAMED)

    private:
        EntityID m_entityID;
        EntityID m_newEntityID;
    };
}