#pragma once
#include "ChaosCore/GameObjects/GameObject.h"
#include "ChaosCore/Resources/SkeletalMesh.h"
#include "ChaosCore/Resources/Material.h"
namespace Chaos
{
	static uint32_t incrementor = 0;
	class SkeletalAnimationGameObject : public GameObject
	{
	public:
		SkeletalAnimationGameObject(const ::Chaos::EntityID& p_id, Resources::SkeletalMeshPtr p_skelMesh, Resources::MaterialPtr p_material);
		~SkeletalAnimationGameObject() = default;
	};
}