#include "ChaosCore/ECS/Managers/ComponentManager.h"

#include "ChaosCore/Components/BoxColliderComponent.h"
#include "ChaosCore/Components/SphereColliderComponent.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosCore/Components/CameraComponent.h"
#include "ChaosCore/Components/ChildComponent.h"
#include "ChaosCore/Components/LightComponent.h"
#include "ChaosCore/Components/MaterialComponent.h"
#include "ChaosCore/Components/MeshComponent.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Components/AudioSourceComponent.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"
#include "ChaosCore/Components/AnimationComponent.h"
#include "ChaosCore/Events/ECSEvents.h"

namespace Chaos
{

    ComponentManager::ComponentManager()
    {
        Expose();
        IComponent::Expose();
        ScriptedComponent::Expose();
        TransformComponent::Expose();
        SphereColliderComponent::Expose();
        RigidBodyComponent::Expose();
        AudioSourceComponent::Expose();
        CameraComponent::Expose();
        MaterialComponent::Expose();
        LightComponent::Expose();
    }

    ComponentManager::~ComponentManager()
    {
    }
    
    ScriptedComponent * ComponentManager::GetScriptedComponent(const EntityID & p_entityID, const std::string & p_componentTypeName)
    {
        const ComponentTypeID CTID = operator""_sid(p_componentTypeName.c_str(), p_componentTypeName.size());

        auto table1 = m_entityComponentTable.find(p_entityID);
        if (table1 != m_entityComponentTable.end())
        {
            auto table2 = table1->second.find(CTID);
            if (table2 != table1->second.end())
            {
                auto component = m_componentTable.find(table2->second);
                if (component != m_componentTable.end())
                {
                    return dynamic_cast<ScriptedComponent*>(component->second.get());
                }
            }
        }
        return nullptr;
    }

    IComponent * ComponentManager::GetComponent(const EntityID p_entityID, const ComponentID p_componentID)
    {
        auto result = m_componentTable.find(p_componentID);
        if (result != m_componentTable.end())
        {
            auto component = result->second;
            auto table1 = m_entityComponentTable.find(p_entityID);
            if (table1 != m_entityComponentTable.end())
            {
                auto table2 = table1->second.find(component->GetComponentTypeID());
                if (table2 != table1->second.end())
                {
                    return component.get();
                }
            }

        }
        return nullptr;
    }

    IComponent* ComponentManager::GetComponent(const ComponentID p_componentID)
    {
        auto result = m_componentTable.find(p_componentID);
        if (result != m_componentTable.end())
        {
            return result->second.get();
        }
        return nullptr;
    }

    std::unordered_map<ComponentTypeID, ComponentID>& ComponentManager::GetAllComponents(const EntityID p_entityID)
    {
        return m_entityComponentTable[p_entityID];
    }

    void ComponentManager::MoveAllComponents(const EntityID p_entityID, const EntityID p_newEntityID)
    {

        auto result = m_entityComponentTable.find(p_entityID);
        if (result != m_entityComponentTable.end())
        {
            auto& table = m_entityComponentTable.at(p_entityID);

            for (auto& p_componentPair : table)
                m_componentTable.at(p_componentPair.second)->m_owner_ID = p_newEntityID;

            m_entityComponentTable[p_newEntityID] = table;
            m_entityComponentTable.erase(p_entityID);
        }
    }

    void ComponentManager::RemoveComponent(ComponentPtr p_component)
    {
        const ComponentTypeID CTID = p_component->GetComponentTypeID();
        const ComponentID componentID = p_component->GetComponentID();

        auto result = m_entityComponentTable.find(p_component->GetOwnerID());
        if (result != m_entityComponentTable.end())
            result->second.erase(CTID);

        auto result2 = m_componentTypedTables.find(CTID);
        if (result2 != m_componentTypedTables.end())
            result2->second->DestroyComponent(componentID);
        m_componentTable.erase(componentID);
    }

    void ComponentManager::Expose()
    {
        ssq::VM& vm = ScriptsHandler::Get().GetVirtualMachine();
        ssq::Table componentManagerTable = vm.addTable("ComponentManager");
        componentManagerTable.addFunc("ForeachComponent", [&](std::string component, ssq::Function func, ssq::SqWeakRef ref)
        {
            auto& compManager = ComponentManager::Get();
            for (auto scriptIt = compManager.begin<ScriptedComponent>(); scriptIt != compManager.end<ScriptedComponent>(); ++scriptIt)
            {
                if (scriptIt->second->GetComponentTypeID() == operator""_sid(component.c_str(), component.size()))
                {
                    vm.callFunc(func, ref, scriptIt->second->GetSqInstance());
                }
            }
        });
    }

	void ComponentManager::RemoveComponent(const EntityID & p_entityID, ComponentTypeID p_type)
	{
		const ComponentID componentID	= m_entityComponentTable.at(p_entityID).at(p_type);
		Events::EventHandler::Get().Send<Events::ComponentAboutToBeRemovedEvent>(*m_componentTable.at(componentID));
		m_componentPendingDeletion.push(m_componentTable.at(componentID));
		m_componentTable.erase(componentID);
	}

	void ComponentManager::RemoveAllComponents(const EntityID& p_entityID)
    {
        auto result = m_entityComponentTable.find(p_entityID);
        if (result != m_entityComponentTable.end())
        {
            for(const std::pair<ComponentTypeID, ComponentID>& i : m_entityComponentTable.at(p_entityID))
            {
                Events::EventHandler::Get().Send<Events::ComponentAboutToBeRemovedEvent>(*m_componentTable.at(i.second));
                m_componentPendingDeletion.push(m_componentTable.at(i.second));
            };

            m_entityComponentTable.erase(p_entityID);
        }
    }

    void ComponentManager::ProcessPendingDeletions()
    {
        auto component = m_componentPendingDeletion.pop();
        while (component)
        {
            RemoveComponent(*component);
            component = m_componentPendingDeletion.pop();
        }
    }

    IComponent& ComponentManager::LoadComponent(ReadStream& p_stream, const EntityID& p_entityID, const ComponentTypeID& p_typeID)
    {
        if (p_typeID == BoxColliderComponent::COMPONENT_TYPE_ID)
        {
            return BoxColliderComponent::Load(p_stream, p_entityID);
        }
        else if (p_typeID == SphereColliderComponent::COMPONENT_TYPE_ID)
        {
            return SphereColliderComponent::Load(p_stream, p_entityID);
        }
        else if (p_typeID == CameraComponent::COMPONENT_TYPE_ID)
        {
            return CameraComponent::Load(p_stream, p_entityID);
        }
        else if (p_typeID == ChildComponent::COMPONENT_TYPE_ID)
        {
            return ChildComponent::Load(p_stream, p_entityID);
        }
        else if (p_typeID == LightComponent::COMPONENT_TYPE_ID)
        {
            return LightComponent::Load(p_stream, p_entityID);
        }
        else if (p_typeID == MaterialComponent::COMPONENT_TYPE_ID)
        {
            return MaterialComponent::Load(p_stream, p_entityID);
        }
        else if (p_typeID == MeshComponent::COMPONENT_TYPE_ID)
        {
            return MeshComponent::Load(p_stream, p_entityID);
        }
        else if (p_typeID == RigidBodyComponent::COMPONENT_TYPE_ID)
        {
            return RigidBodyComponent::Load(p_stream, p_entityID);
        }
        else if (p_typeID == TransformComponent::COMPONENT_TYPE_ID)
        {
            return TransformComponent::Load(p_stream, p_entityID);
        }
        else if (p_typeID == AudioSourceComponent::COMPONENT_TYPE_ID)
        {
            return AudioSourceComponent::Load(p_stream, p_entityID);
        }
		else if (p_typeID == AnimationComponent::COMPONENT_TYPE_ID)
		{
			return AnimationComponent::Load(p_stream, p_entityID);
		}
        else
        {
            return ScriptedComponent::Load(p_stream, p_entityID);
        }
    }
}