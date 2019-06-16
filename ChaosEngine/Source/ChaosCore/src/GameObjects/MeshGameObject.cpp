
#include "ChaosCore/GameObjects/MeshGameObject.h"
#include "ChaosCore/GameObjects/EmptyGameObject.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Components/MeshComponent.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Components/MaterialComponent.h"

namespace Chaos
{
    MeshGameObject::MeshGameObject(const ::Chaos::EntityID& p_id, Resources::MeshPtr p_mesh, Resources::MaterialPtr p_material) : GameObject(p_id)
    {
        if (p_mesh->GetSubMeshes().size() > 1)
        {
            std::unordered_map<std::string, EmptyGameObject&> createdSubMeshes(p_mesh->GetSubMeshes().size());
            for (Resources::SubMeshPtr submesh : p_mesh->GetSubMeshes())
            {
                auto sceneSystem = SystemManager::Get().GetSystem<SceneSystem>();
                EmptyGameObject& empty = sceneSystem->CreateEntity<EmptyGameObject>(operator""_sid(submesh->GetName().c_str(), submesh->GetName().size()));
                empty.GetComponent<TransformComponent>()->AsTransform().SetTransform(submesh->GetTransformation());
                empty.AddComponent<MeshComponent>(submesh);
                empty.AddComponent<MaterialComponent>(p_material);
                createdSubMeshes.emplace(submesh->GetName(),empty);

                if (!submesh->GetParent().empty() && submesh->GetParent() != "RootNode")
                {
                    sceneSystem->DelayedSetParent(empty.GetEntityID(), createdSubMeshes.at(submesh->GetParent()).GetEntityID(), sceneSystem->GetCurrentScene());
                }
                else
                {
                    sceneSystem->DelayedSetParent(empty.GetEntityID(), m_entityID, sceneSystem->GetCurrentScene());
                }
            }
        }
        else
        {
            AddComponent<MeshComponent>(p_mesh->GetSubMeshes()[0]);
        }
        AddComponent<MaterialComponent>(p_material);
    }
}