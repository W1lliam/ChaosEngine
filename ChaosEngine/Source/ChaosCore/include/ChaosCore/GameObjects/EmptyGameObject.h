#pragma once
#include "ChaosCore/GameObjects/GameObject.h"

namespace Chaos
{
	class EmptyGameObject : public GameObject
	{
	public:
		EmptyGameObject(const ::Chaos::EntityID& p_id);
		~EmptyGameObject() = default;
	};
}