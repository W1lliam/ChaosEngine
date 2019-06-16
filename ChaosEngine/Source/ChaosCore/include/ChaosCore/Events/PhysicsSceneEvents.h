#pragma once
#include "ChaosCore/Events/AEvent.h"
#include "KeyCodes.h"

namespace Chaos::Events
{
	/*class APhysicsSceneEvent : public AEvent
	{
	public:
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_SCENE)
			RigidBodyComponent& GetrigidBodyComponent() const { return m_rigidBody; }
	protected:
		APhysicsSceneEvent(E_EventType eType, RigidBodyComponent& p_rigidBody) : AEvent(eType), m_rigidBody(p_rigidBody) {}
		RigidBodyComponent& m_rigidBody;
	};

	class RigidBodyCreatedEvent : public APhysicsSceneEvent
	{
	public:
		RigidBodyCreatedEvent(RigidBodyComponent& p_rigidBody) : APhysicsSceneEvent(E_EventType::RIGIDBODY_CREATED, p_rigidBody) {}
		inline void Print() const override
		{
			CHAOS_CORE_INFO("RigidBodyCreatedEvent:\n\tPhysicsScene {0} Created RigidBody {1}");
		}
		EVENT_CLASS_TYPE(RIGIDBODY_CREATED)
	};
	class RigidBodyRemovedEvent : public APhysicsSceneEvent
	{
	public:
		RigidBodyRemovedEvent(RigidBodyComponent& p_rigidBody) : APhysicsSceneEvent(E_EventType::RIGIDBODY_REMOVED, p_rigidBody) {}

		inline void Print() const override
		{
			CHAOS_CORE_INFO("RigidBodyRemovedEvent:\n\tPhysicsScene {0} Removed RigidBody {1}");
		}
		EVENT_CLASS_TYPE(RIGIDBODY_REMOVED)
	};*/
}
