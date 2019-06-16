#pragma once
#include "ChaosCore/GameObjects/GameObject.h"

namespace Chaos
{
    class CameraGameObject : public GameObject
    {
    public:
        CameraGameObject(const ::Chaos::EntityID& p_id);
        ~CameraGameObject() = default;
    };
}