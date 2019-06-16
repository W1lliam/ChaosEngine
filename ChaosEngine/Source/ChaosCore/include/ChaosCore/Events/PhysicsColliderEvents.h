#pragma once
#include "ChaosCore/Events/AEvent.h"
#include "KeyCodes.h"
#include "ChaosCore/Physics/PhysicsScene.h"
#include "ChaosCore/Components/BoxColliderComponent.h"
#include "ChaosCore/Components/SphereColliderComponent.h"

namespace Chaos::Events
{
	class APhysicsColliderEvent : public AEvent
	{
	public:
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_SCENE)
	protected:
		APhysicsColliderEvent(E_EventType eType) : AEvent(eType) {}
	};

	class BoxColliderRemovedEvent : public APhysicsColliderEvent
	{
	public:
		BoxColliderRemovedEvent(BoxColliderComponent& p_boxCollider) : APhysicsColliderEvent(E_EventType::BOXCOLLIDER_REMOVED), m_boxCollider(p_boxCollider) {}
		BoxColliderComponent& GetCollider() { return m_boxCollider; }
		inline void Print() const override
		{
			CHAOS_CORE_INFO("BoxColliderRemoveEvent:\n\tPhysicsScene {0} Removed BoxCollider {1}");
		}
		EVENT_CLASS_TYPE(RIGIDBODY_CREATED)
	private:
		BoxColliderComponent& m_boxCollider;

	};
	class SphereColliderRemovedEvent : public APhysicsColliderEvent
	{
	public:
		SphereColliderRemovedEvent(SphereColliderComponent& p_sphereCollider) : APhysicsColliderEvent(E_EventType::SPHERECOLLIDER_REMOVED), m_sphereCollider(p_sphereCollider) {}
		SphereColliderComponent& GetCollider() { return m_sphereCollider; }
		inline void Print() const override
		{
			CHAOS_CORE_INFO("SphereColliderRemovedEvent:\n\tPhysicsScene {0} Removed SphereCollider {1}");
		}
		EVENT_CLASS_TYPE(RIGIDBODY_REMOVED)
	private:
		SphereColliderComponent& m_sphereCollider;
	};
}