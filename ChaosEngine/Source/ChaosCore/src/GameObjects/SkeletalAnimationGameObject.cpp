#include "ChaosCore/GameObjects/SkeletalAnimationGameObject.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/GameObjects/EmptyGameObject.h"
#include "ChaosCore/Components/AnimationComponent.h" 
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Components/MaterialComponent.h"

namespace Chaos
{

	Chaos::SkeletalAnimationGameObject::SkeletalAnimationGameObject(const::Chaos::EntityID & p_id, Resources::SkeletalMeshPtr p_skelMesh, Resources::MaterialPtr p_material) : GameObject(p_id)
	{
		if(p_skelMesh)
		{ 
			if (p_skelMesh->GetAnimationMesh().size() > 0)
			{
				uint32_t id = 0;
				AddComponent<AnimationComponent>(p_skelMesh);
				std::unordered_map<uint32_t, EmptyGameObject&> createdSubMeshes(p_skelMesh->GetAnimationMesh().size());
				for (Resources::AnimMeshPtr submesh : p_skelMesh->GetAnimationMesh())
				{
					auto sceneSystem = SystemManager::Get().GetSystem<SceneSystem>();
					std::string name = submesh->GetAnimMeshName() + std::to_string(incrementor);
					EmptyGameObject& empty = sceneSystem->CreateEntity<EmptyGameObject>(operator""_sid(name.c_str(), name.size()));
					empty.AddComponent<MaterialComponent>(p_material);
					EntityID entityid = empty.GetEntityID();
					p_skelMesh->GetAnimationMesh()[id]->SetOwnerID(entityid);
					createdSubMeshes.emplace(id, empty);
					if (id != 0)
					{
						sceneSystem->DelayedSetParent(empty.GetEntityID(), createdSubMeshes.at(id - 1).GetEntityID(), sceneSystem->GetCurrentScene());
					}
					else
					{
						sceneSystem->DelayedSetParent(empty.GetEntityID(), m_entityID, sceneSystem->GetCurrentScene());
					}
					++id;
					++incrementor;
				}
			}
		}
	}
}