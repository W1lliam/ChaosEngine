#include "ChaosCore/Components/ChildComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"

namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(ChildComponent);

    ChildComponent::ChildComponent(COMPONENT_INIT_PARAMS, EntityID p_parent) : COMPONENT_INIT(ChildComponent), m_parent(p_parent)
    {
    }

    bool ChildComponent::Save(WriteStream & p_stream)
    {
        SerializeStringID(p_stream, m_parent);
        return true;
    }

    IComponent& ChildComponent::Load(ReadStream & p_stream, const EntityID & p_entityID)
    {
        StringID parent;
        SerializeStringID(p_stream, parent);
        return  ComponentManager::Get().AddComponent<ChildComponent>(p_entityID, parent);
    }
}