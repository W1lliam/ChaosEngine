#pragma once
#include "ChaosCore/GameObjects/GameObject.h"

namespace Chaos
{
	class LightGameObject : public GameObject
	{
	public:
		LightGameObject(const ::Chaos::EntityID& p_id);
		~LightGameObject() = default;
	};
}