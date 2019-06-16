#pragma once
#include "ChaosCore/Events/AEvent.h"
#include "KeyCodes.h"
#include "ChaosCore/Scene/Scene.h"

namespace Chaos::Events
{
    class ASceneEvent : public AEvent
    {
    public:
        inline Scene& GetScene() const { return m_scene; }

        EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_SCENE)

    protected:
        ASceneEvent(E_EventType eType, Scene& p_scene) : AEvent(eType), m_scene(p_scene) {}
        Scene& m_scene;
    };

    class EntityCreatedEvent : public ASceneEvent
    {
    public:
        EntityCreatedEvent(Scene& p_scene, EntityID p_entityID) : ASceneEvent(E_EventType::ENTITY_CREATED, p_scene), m_entityID(p_entityID) {}
        inline EntityID GetEntityID() const { return m_entityID; }

        inline void Print() const override
        {
            CHAOS_CORE_INFO("EntityCreatedEvent:\n\tScene {0} Created Entity {1}", m_scene.GetName(), m_entityID);
        }
        EVENT_CLASS_TYPE(ENTITY_CREATED)

    private:
        EntityID m_entityID;
    };
    class EntityRemovedEvent : public ASceneEvent
    {
    public:
        EntityRemovedEvent(Scene& p_scene, EntityID p_entityID) : ASceneEvent(E_EventType::ENTITY_REMOVED, p_scene), m_entityID(p_entityID) {}
        inline EntityID GetEntityID() const { return m_entityID; }

        inline void Print() const override
        {
            CHAOS_CORE_INFO("EntityRemovedEvent:\n\tScene {0} Removed Entity {1}", m_scene.GetName(), m_entityID);
        }
        EVENT_CLASS_TYPE(ENTITY_REMOVED)

    private:
        EntityID m_entityID;
    };

	class SceneChangedEvent : public ASceneEvent
	{
	public:
		SceneChangedEvent(Scene& p_scene) : ASceneEvent(E_EventType::SCENE_CHANGED, p_scene) {}

		inline void Print() const override
		{
			CHAOS_CORE_INFO("SceneChangedEvent:\n\tScene {0}", m_scene.GetName());
		}
		EVENT_CLASS_TYPE(SCENE_CHANGED)

	private:
	};
}