#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/Events/EventHandler.h"
#include "ChaosCore/Events/ECSEvents.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/ECS/Entity/AEntity.h"

namespace Chaos
{
    IComponent::IComponent(const EntityID& p_owner_ID, const ComponentID & p_componentID) : m_owner_ID(p_owner_ID),
        m_component_ID(p_componentID), m_enabled(true)
    {
        Events::EventHandler::Get().Send<Events::ComponentCreatedEvent>(*this);
    }
    void IComponent::Expose()
    {
        ssq::Class sqComponent = ScriptsHandler::Get().GetVirtualMachine().addAbstractClass<IComponent>("Component");
        sqComponent.addFunc("GetOwner", [](IComponent* p_self) -> AEntity* 
        {
            return EntityManager::Get().GetEntity(p_self->GetOwnerID()).get();
        });
    }
}