#include "ChaosCore/Components/MeshComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/Resources/Managers/MeshManager.h"

namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(MeshComponent);

    MeshComponent::MeshComponent(COMPONENT_INIT_PARAMS, Resources::SubMeshPtr p_mesh) :
        COMPONENT_INIT(MeshComponent), m_mesh(p_mesh)
    {
    }

    void MeshComponent::SetMesh(Resources::SubMeshPtr p_mesh)
    {
        if (p_mesh)
            m_mesh = p_mesh;
    }

    const Resources::SubMeshPtr MeshComponent::GetMesh() const
    {
        return m_mesh;
    }
    
    bool MeshComponent::Save(WriteStream & p_stream)
    {
        SerializeStringID(p_stream, const_cast<StringID&>(m_mesh->GetOwner()->GetGroup()));
        SerializeStringID(p_stream, const_cast<StringID&>(m_mesh->GetOwner()->GetName()));
        SerializeString(p_stream, const_cast<std::string&>(m_mesh->GetName()));
        return true;
    }

    IComponent& MeshComponent::Load(ReadStream & p_stream, const EntityID & p_entityID)
    {
        StringID    meshGroup;
        StringID    meshName;
        std::string subMeshName;
        SerializeStringID(p_stream, meshGroup);
        SerializeStringID(p_stream, meshName);
        SerializeString(p_stream, subMeshName);

        Resources::MeshPtr mesh = MeshManager::Get().GetMesh(meshName, meshGroup);
        if (mesh)
        {
            return ComponentManager::Get().AddComponent<MeshComponent>(p_entityID, mesh->GetSubMesh(subMeshName));
        }
        CHAOS_CORE_WARN("Mesh {0} not found in group {1}", meshName, meshGroup);
        return ComponentManager::Get().AddComponent<MeshComponent>(p_entityID, nullptr);
    }
}