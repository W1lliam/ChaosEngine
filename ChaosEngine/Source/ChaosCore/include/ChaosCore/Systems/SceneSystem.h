#pragma once 
#pragma once
#include <ChaosMaths.h>
#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/ECS/Entity/AEntity.h"
#include "ChaosCore/Scene/Scene.h"
#include "ChaosCore/Multithreading/Containers/LockFree/LFQueue.h"

namespace Chaos
{
    class ChildComponent;
    class SceneSystem : public System
    {
        CHAOS_SYSTEM(SceneSystem);
    public:
        SceneSystem(ScenePtr p_scene = {});
        ~SceneSystem() = default;
        
        void        Start()                 override;
        void        PreUpdate(double dt)    override;
        void        Update(double dt)		override;
        void        PostUpdate(double dt)   override;

        template<class T, class... Args>
        T& CreateEntity(const EntityID& p_entityID, Args&&... args)
        {
            if (!m_currentScene)
                throw new std::exception("No active scene currently set!");
            EntityPtr entity = EntityManager::Get().CreateEntity<T>(p_entityID, std::forward<Args>(args)...);
            m_currentScene->AddEntity(entity);
            return *std::reinterpret_pointer_cast<T>(entity);
        }

        template<class T, class... Args>
        T& CreateChildEntity(const EntityID& p_entityID, const EntityID& p_parentID, Args&&... args)
        {
            if (!m_currentScene)
                throw new std::exception("No active scene currently set!");

            EntityPtr parent = EntityManager::Get().GetEntity(p_parentID);
            EntityPtr entity = EntityManager::Get().CreateEntity<T>(p_entityID, std::forward<Args>(args)...);
            m_currentScene->AddEntity(entity, parent);
            return *std::reinterpret_pointer_cast<T>(entity);
        }

        void SetCurrentScene(const StringID& p_name, const StringID& p_group);
        void SetCurrentScene(const ScenePtr& p_scene);
        void DelayedSetParent(const EntityID& p_name, const EntityID& p_group, ScenePtr p_scene = ScenePtr());

        template<class T, class... Args>
        T* AddSystem(Args&&... args)
        {
            T* system = SystemManager::Get().AddSystem<T>(std::forward<Args>(args)...);
            m_currentScene->AddSystem(system);
            return system;
        }
        
        System* AddSystem(const SystemTypeID& p_systemTypeID);
        
        template<class T>
        void RemoveSystem()
        {
            m_currentScene->RemoveSystem<T>();
            SystemManager::Get().DisableSystem<T>();
        }

        void RemoveSystem(System* p_system);
        void RemoveSystem(const SystemTypeID& p_systemTypeID);

        template<class T>
        T* GetSystem()
        {
            return m_currentScene->GetSystem<T>();
        }

        void AddSystem(System* p_system);

        static Maths::Matrix4   GetWorldTransform(EntityID p_entityID);
        inline ScenePtr         GetCurrentScene() { return m_currentScene; }

        virtual bool    Save(WriteStream& p_stream) override;
        static  System& Load(ReadStream & p_stream);

    private:
        static  bool PropagateDirtyState(ChildComponent* p_child);
                void ProcessPendingInsertionChilds();

                void Expose();
    private:
        ScenePtr m_currentScene;
        Containers::LockFree::LFQueue<std::tuple<EntityID, EntityID, ScenePtr>> m_childPendingInsertion;
    };
}