#pragma once
#include "ChaosCore/GameObjects/GameObject.h"
#include "ChaosCore/Resources/Mesh.h"
#include "ChaosCore/Resources/Material.h"

namespace Chaos
{
	class MeshGameObject : public GameObject
	{
	public:
        MeshGameObject(const ::Chaos::EntityID& p_id, Resources::MeshPtr p_mesh, Resources::MaterialPtr p_material);
		~MeshGameObject() = default;
	};
}