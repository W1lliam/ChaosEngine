#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Components/ChildComponent.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/GameObjects/EmptyGameObject.h"
#include "ChaosCore/GameObjects/CameraGameObject.h"
#include "ChaosCore/GameObjects/MeshGameObject.h"
#include "ChaosCore/GameObjects/SkeletalAnimationGameObject.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Resources/Managers/SceneManager.h"
#include "ChaosCore/Events/EventHandler.h"
#include "ChaosCore/Events/SceneEvents.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"
#include "ChaosCore/Resources/Managers/AnimationManager.h"
#include "ChaosCore/Resources/Managers/MeshManager.h"
#include "ChaosCore/Resources/Managers/MaterialManager.h"

namespace Chaos
{
    CHAOS_DEFINE_SYSTEM_TYPEID(SceneSystem);

    SceneSystem::SceneSystem(ScenePtr p_scene) : System(System::NORMAL_SYSTEM_PRIORITY, E_SystemExecutionModeFlags::BOTH),
        m_currentScene(p_scene)
    {
        if(m_currentScene)
            m_currentScene->EnableSystems();
        Expose();
    }

    void SceneSystem::Start()
    {
        if (m_currentScene)
            m_currentScene->EnableSystems();

    }

    void SceneSystem::PreUpdate(double dt)
    {
        for (auto childIt = m_currentScene->begin<ChildComponent>(); childIt != m_currentScene->end<ChildComponent>(); childIt++)
        {
            PropagateDirtyState(reinterpret_cast<ChildComponent*>(childIt->second));
        };
        ProcessPendingInsertionChilds();
    }

    void SceneSystem::Update(double dt)
    {
        for (auto childIt = m_currentScene->begin<ChildComponent>(); childIt != m_currentScene->end<ChildComponent>(); childIt++)
        {
            PropagateDirtyState(reinterpret_cast<ChildComponent*>(childIt->second));
        };
        ProcessPendingInsertionChilds();
    }

    void SceneSystem::PostUpdate(double dt)
    {
        for (auto childIt = m_currentScene->begin<ChildComponent>(); childIt != m_currentScene->end<ChildComponent>(); childIt++)
        {
            PropagateDirtyState(reinterpret_cast<ChildComponent*>(childIt->second));
        };
        ProcessPendingInsertionChilds();
    }

    void SceneSystem::SetCurrentScene(const StringID & p_name, const StringID & p_group)
    {
        auto scene = SceneManager::Get().GetScene(p_name, p_group);
        if (scene)
        {
            m_currentScene->DisableSystems();
            m_currentScene = scene;
            m_currentScene->EnableSystems();
            Events::EventHandler::Get().Send<Events::SceneChangedEvent>(*m_currentScene);
        }
    }

    void SceneSystem::SetCurrentScene(const ScenePtr& p_scene)
    {
        if (p_scene)
        {
            m_currentScene->DisableSystems();
            m_currentScene = p_scene;
            m_currentScene->EnableSystems();
            Events::EventHandler::Get().Send<Events::SceneChangedEvent>(*m_currentScene);
        }
    }

    void SceneSystem::DelayedSetParent(const EntityID& p_child, const EntityID& p_parent, ScenePtr p_scene)
    {
        m_childPendingInsertion.push(std::make_tuple(p_child, p_parent, p_scene));
    }

    System * SceneSystem::AddSystem(const SystemTypeID & p_systemTypeID)
    {
        System* system = SystemManager::Get().GetSystem(p_systemTypeID).get();
        m_currentScene->AddSystem(system);
        return system;
    }

    void SceneSystem::RemoveSystem(System* p_system)
    {
        p_system->Disable();
        m_currentScene->RemoveSystem(p_system);
    }

    void SceneSystem::RemoveSystem(const SystemTypeID & p_systemTypeID)
    {
        SystemManager::Get().DisableSystem(p_systemTypeID);
        m_currentScene->RemoveSystem(p_systemTypeID);
    }

    void SceneSystem::AddSystem(System* p_system)
    {
        m_currentScene->AddSystem(p_system);
    }

    Maths::Matrix4 SceneSystem::GetWorldTransform(EntityID p_entityID)
    {
        static  ComponentManager& compManager = ComponentManager::Get();
        TransformComponent* transformComp   = compManager.GetComponent<TransformComponent>(p_entityID);
        if (transformComp)
        {
            Maths::Matrix4      transform = transformComp->AsTransform().GetTransform();
            ChildComponent* child = compManager.GetComponent<ChildComponent>(p_entityID);
            if (child != nullptr)
            {
                if (child->IsDirty())
                    child->SetCachedTransform(GetWorldTransform(child->GetParentID()) * transform);
                return child->GetCachedTransform();
            }
            return transform;
        }
        return Maths::Matrix4{};
    }

    bool SceneSystem::Save(WriteStream& p_stream)
    {
        StringID    sceneID     = m_currentScene->GetName();
        StringID    groupID     = m_currentScene->GetGroup();
        SerializeStringID(p_stream, sceneID);
        SerializeStringID(p_stream, groupID);
        return System::Save(p_stream);
    }

    System& SceneSystem::Load(ReadStream & p_stream)
    {
        StringID    sceneID;
        StringID    groupID;
        SerializeStringID(p_stream, sceneID);
        SerializeStringID(p_stream, groupID);
        ScenePtr scene = SceneManager::Get().GetScene(sceneID, groupID);

        SceneSystem* system = SystemManager::Get().GetSystem<SceneSystem>().get();
        if (system)
        {
            system->SetCurrentScene(scene);
        }
        else
        {
            system = SystemManager::Get().AddSystem<SceneSystem>(scene);
        }
        static_cast<System*>(system)->Load(p_stream);
        return *system;
    }

    bool SceneSystem::PropagateDirtyState(ChildComponent* p_child)
    {
        static  ComponentManager& compManager = ComponentManager::Get();
        ChildComponent* parentChildComponent = compManager.GetComponent<ChildComponent>(p_child->GetParentID());
        if (parentChildComponent == nullptr)
        {
            auto transformComp = compManager.GetComponent<TransformComponent>(p_child->GetParentID());
            if(transformComp)
                p_child->SetDirty(transformComp->AsTransform().IsDirty());
        }
        else
        {
            p_child->SetDirty(PropagateDirtyState(parentChildComponent));
        }
        return p_child->IsDirty();
    }

    void SceneSystem::ProcessPendingInsertionChilds()
    {
        auto childPair = m_childPendingInsertion.pop();
        while (childPair)
        {
            if (!std::get<2>(*childPair))
                m_currentScene->SetParent(std::get<0>(*childPair), std::get<1>(*childPair));
            else
                std::get<2>(*childPair)->SetParent(std::get<0>(*childPair), std::get<1>(*childPair));
            childPair = m_childPendingInsertion.pop();
        }
    }

    void SceneSystem::Expose()
    {
        Scene::Expose();

        ssq::Table sceneTable = ScriptsHandler::Get().GetVirtualMachine().addTable("SceneSystem");
        sceneTable.addFunc("GetCurrentScene",       []() -> Scene* { return SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene().get(); });
        sceneTable.addFunc("CreateGameObject",      [](std::string p_name) -> AEntity& { return SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<EmptyGameObject>(operator""_sid(p_name.c_str(), p_name.size())); });
        
        sceneTable.addFunc("CreateCamera",          [](std::string p_name) -> AEntity& { return SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<CameraGameObject>(operator""_sid(p_name.c_str(), p_name.size())); });
        sceneTable.addFunc("CreateChildCamera",     [](AEntity* p_parent, std::string p_name) -> AEntity& 
        { 
            return SystemManager::Get().GetSystem<SceneSystem>()->CreateChildEntity<CameraGameObject>(operator""_sid(p_name.c_str(), p_name.size()), p_parent->GetEntityID());
        });
        sceneTable.addFunc("CreateMeshGameObject",  [](std::string p_name, std::string p_mesh, std::string p_material) -> AEntity&
        { 
            return SystemManager::Get().GetSystem<SceneSystem>()->CreateEntity<MeshGameObject>(
                operator""_sid(p_name.c_str(), p_name.size()), 
                MeshManager::Get().GetMesh(operator""_sid(p_mesh.c_str(), p_mesh.size())),
                std::reinterpret_pointer_cast<Resources::Material>(MaterialManager::Get().CreateOrRetrieve(p_material).first));
        });
    }
}