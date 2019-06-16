#pragma once
#include "ChaosCore/GameObjects/GameObject.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosCore/Physics/Collider.h"

namespace Chaos
{
	class RigidBodyGameObject : public GameObject
	{
	public:
		RigidBodyGameObject(const ::Chaos::EntityID& p_id);
		~RigidBodyGameObject() = default;
	};
}