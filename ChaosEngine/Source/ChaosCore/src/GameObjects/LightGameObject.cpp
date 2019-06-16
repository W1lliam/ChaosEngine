#include "ChaosCore/GameObjects/LightGameObject.h"
#include "ChaosCore/Components/LightComponent.h"

namespace Chaos
{
	LightGameObject::LightGameObject(const ::Chaos::EntityID& p_id) : GameObject(p_id)
	{
		AddComponent<LightComponent>();
	}
}