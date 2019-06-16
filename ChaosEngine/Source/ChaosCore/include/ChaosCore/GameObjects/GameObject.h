#pragma once
#include "ChaosCore/ECS/Entity/AEntity.h"
#include "ChaosCore/Components/TransformComponent.h"

namespace Chaos
{
	class GameObject : public AEntity
	{
	public:
		GameObject(const ::Chaos::EntityID& p_id) : AEntity(p_id)
		{
	        AddComponent<TransformComponent>();
		}

		virtual ~GameObject()
		{
		}
	};
}