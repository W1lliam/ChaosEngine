#include "ChaosCore/ECS/Entity/AEntity.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Components/SphereColliderComponent.h"
#include "ChaosCore/Components/BoxColliderComponent.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosCore/Components/CameraComponent.h"
#include "ChaosCore/Components/ChildComponent.h"
#include "ChaosCore/Components/MaterialComponent.h"
#include "ChaosCore/Components/AudioSourceComponent.h"
#include "ChaosCore/Components/LightComponent.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"
#include "ChaosCore/Events/ECSEvents.h"

namespace Chaos
{
    AEntity::AEntity(const EntityID & p_id) : m_entityID(p_id), m_active(true)
    {
    }

    bool AEntity::Save(WriteStream & p_stream)
    {
        CHAOS_CORE_INFO("Saving {0}", m_entityID);

        SerializeStringID(p_stream, m_entityID);
        SerializeBool(p_stream, m_active);
        std::unordered_map<ComponentTypeID, ComponentID> components = ComponentManager::Get().GetAllComponents(this->m_entityID);
        
        uint32_t componentCount = (uint32_t)components.size();
        SerializeUnsignedInt(p_stream, componentCount, 0, UINT32_MAX);

        auto tranformCompIt = std::find_if(components.begin(), components.end(), [](const std::pair<ComponentTypeID, ComponentID>& p_compPair)
        {
            return p_compPair.first == TransformComponent::COMPONENT_TYPE_ID;
        });
        if (tranformCompIt != components.end())
        {
            ComponentTypeID CTID = tranformCompIt->first;
            SerializeStringID(p_stream, CTID);
            ComponentManager::Get().GetComponent(tranformCompIt->second)->Save(p_stream);
            components.erase(tranformCompIt);
        }

        auto boxColliderCompIt = std::find_if(components.begin(), components.end(), [](const std::pair<ComponentTypeID, ComponentID>& p_compPair)
        {
            return p_compPair.first == BoxColliderComponent::COMPONENT_TYPE_ID;
        });
        if (boxColliderCompIt != components.end())
        {
            ComponentTypeID CTID = boxColliderCompIt->first;
            SerializeStringID(p_stream, CTID);
            ComponentManager::Get().GetComponent(boxColliderCompIt->second)->Save(p_stream);
            components.erase(boxColliderCompIt);
        }

        auto sphereColliderCompIt = std::find_if(components.begin(), components.end(), [](const std::pair<ComponentTypeID, ComponentID>& p_compPair)
        {
            return p_compPair.first == SphereColliderComponent::COMPONENT_TYPE_ID;
        });
        if (sphereColliderCompIt != components.end())
        {
            ComponentTypeID CTID = sphereColliderCompIt->first;
            SerializeStringID(p_stream, CTID);
            ComponentManager::Get().GetComponent(sphereColliderCompIt->second)->Save(p_stream);
            components.erase(sphereColliderCompIt);
        }

        for (const std::pair<ComponentTypeID, ComponentID>& component : components)
        {
            ComponentTypeID CTID = component.first;
            SerializeStringID(p_stream, CTID);
            ComponentManager::Get().GetComponent(component.second)->Save(p_stream);
        }
        return true;
    }

    bool AEntity::Load(ReadStream & p_stream)
    {
        SerializeStringID(p_stream, m_entityID);
        SerializeBool(p_stream, m_active);

        uint32_t componentCount;
        SerializeUnsignedInt(p_stream, componentCount, 0, UINT32_MAX);

        while (componentCount--)
        {
            ComponentTypeID CTID;
            SerializeStringID(p_stream, CTID);
            IComponent& component = LoadComponent(p_stream, CTID);
        }
        return true;
    }

    void AEntity::SetActive(const bool p_active)
    {
        if (this->m_active == p_active)
            return;

        if (!p_active)
            this->OnDisable();
        else
            this->OnEnable();

        this->m_active = p_active;
    }

    void AEntity::Rename(const std::string & p_newName) 
    { 
        Rename(operator""_sid(p_newName.c_str(), p_newName.size())); 
    }

    void AEntity::Rename(const EntityID & p_newName) 
    { 
        Events::EventHandler::Get().Send<Events::EntityRenamedEvent>(m_entityID, p_newName);
    }

    void AEntity::Expose()
    {
        ssq::Class sqEntity = ScriptsHandler::Get().GetVirtualMachine().addClass("Entity", ssq::Class::Ctor<AEntity(EntityID)>());
        sqEntity.addFunc("GetName", &AEntity::GetFullName);
        sqEntity.addFunc("IsActive", &AEntity::IsActive);
        sqEntity.addFunc("GetTransform", [&](AEntity* self) -> TransformComponent* 
        { 
            return self->GetComponent<TransformComponent>();
        });
        sqEntity.addFunc("GetCamera", [&](AEntity* self) -> CameraComponent* { return self->GetComponent<CameraComponent>();});
        sqEntity.addFunc("GetRigidBody", [&](AEntity* self) -> RigidBodyComponent* { return self->GetComponent<RigidBodyComponent>();});
        sqEntity.addFunc("GetMaterial", [&](AEntity* self) -> MaterialComponent* { return self->GetComponent<MaterialComponent>();});
        sqEntity.addFunc("GetComponent", [&](AEntity* self, std::string p_component) -> ssq::Instance
        {
            return self->GetScriptedComponent(p_component)->GetSqInstance();
        });

        sqEntity.addFunc("AddSphereCollider", [&](AEntity* self, Maths::Vector3 p_friction, Maths::Vector3 p_offSet, float p_radius) -> SphereColliderComponent*
        {
            return &self->AddComponent<SphereColliderComponent>(p_friction, p_offSet, p_radius);
        });

        sqEntity.addFunc("AddBoxCollider", [&](AEntity* self, Maths::Vector3 p_friction, Maths::Vector3 p_offSet, Maths::Vector3 p_halfExtends) -> BoxColliderComponent*
        {
            return &self->AddComponent<BoxColliderComponent>(p_friction, p_offSet, p_halfExtends);
        });

        sqEntity.addFunc("AddRigidBody", [&](AEntity* self, int p_type, float p_mass, Maths::Vector3 p_velocity) -> RigidBodyComponent*
        {
            return &self->AddComponent<RigidBodyComponent>((E_RigidBodyType)p_type, p_mass, p_velocity);
        });
        sqEntity.addFunc("AddEmptyAudioSource", [&](AEntity* self) -> AudioSourceComponent*
        {
            return &self->AddComponent<AudioSourceComponent>();
        });
        sqEntity.addFunc("AddAudioSource", [&](AEntity* self, std::string p_clipName, std::string p_clipGroup) -> AudioSourceComponent*
        {
            Resources::AudioClipPtr audioclip = std::reinterpret_pointer_cast<Resources::AudioClip>(AudioResourcesManager::Get().CreateOrRetrieve(p_clipName, operator""_sid(p_clipGroup.c_str(), p_clipGroup.size())).first);
            return &self->AddComponent<AudioSourceComponent>(audioclip);
        });
        sqEntity.addFunc("AddLight", [&](AEntity* self) -> LightComponent*
        {
            return &self->AddComponent<LightComponent>();
        });
    }
}