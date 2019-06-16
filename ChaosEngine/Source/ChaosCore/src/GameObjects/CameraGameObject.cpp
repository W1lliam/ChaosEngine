#include "ChaosCore/GameObjects/CameraGameObject.h"
#include "ChaosCore/Components/CameraComponent.h"

namespace Chaos
{
    CameraGameObject::CameraGameObject(const ::Chaos::EntityID& p_id) : GameObject(p_id)
    {
        AddComponent<CameraComponent>();
    }
}