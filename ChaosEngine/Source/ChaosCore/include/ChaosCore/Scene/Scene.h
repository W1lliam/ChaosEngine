#pragma once
#include "ChaosCore/chaos_core_pch.h"
#include <ChaosPhysics/Scene/Scene.h>
#include "ChaosCore/Utils/StringID.h"
#include "ChaosCore/ECS/Entity/AEntity.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"
#include "ChaosCore/Multithreading/Containers/ThreadSafeList.h"
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Events/ECSEvents.h"
#include "ChaosCore/Resources/ASkybox.h"
#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/Serialization/Serializer.h"
#include "ChaosCore/Containers/RHHashTable.h"


namespace Chaos
{
	struct SceneRenderParam 
	{
		uint8_t	bloomBlurPasses		= 0;
		float	bloomThreshold		= 50.0f;
		float	bloomIntesity		= 0.0f;
		float	gamma				= 1.5f;
		float	skyboxExposure      = 1.0f;
	};

    class Scene : public AResource, public Events::EventListener, public ISerializable
    {
    friend class SceneManager;

    public:
        struct SceneNode
        {
            std::vector<SceneNode*> childs  {};
            SceneNode*              parent  {nullptr};
            EntityPtr               entity  {nullptr};

            SceneNode()     = default;
            ~SceneNode()    = default;

            SceneNode(SceneNode* p_parent) : parent(p_parent)
            {}

            SceneNode(EntityPtr& p_entity, SceneNode* p_parent = nullptr) : entity(p_entity), parent(p_parent)
            {}
        };

        Scene(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group);
        ~Scene();

		inline const SceneRenderParam& GetRenderParams() const					{ return m_sceneParams;								}
		
		inline void SetRenderParams(const SceneRenderParam& p_renderParams)		{ m_sceneParams					= p_renderParams;	}
		inline void SetBloomThreshold(const float p_bloom)						{ m_sceneParams.bloomThreshold	= p_bloom;			}
		inline void SetGamma(const float p_gamma)								{ m_sceneParams.gamma			= p_gamma;			}
		inline void SetBloomBlurPasses(const uint8_t p_bloomBlur)				{ m_sceneParams.bloomBlurPasses = p_bloomBlur;		}
		inline void SetBloomIntensity(const float p_bloomIntensity)				{ m_sceneParams.bloomIntesity   = p_bloomIntensity;	}
        inline void	SetExposure(float p_exposure)                               { m_sceneParams.skyboxExposure  = p_exposure;       };
		inline void SetSkybox(const Resources::SkyboxPtr& p_skybox)             { m_skybox = p_skybox;	                            };

        void AddEntity(EntityPtr p_entity, EntityPtr p_parent = {});
        void SetParent(const EntityID& p_childID, const EntityID& p_parentID);
        void SetParent(EntityPtr p_child, EntityPtr p_parent);
        void RemoveEntity(EntityPtr p_entity);

        void Clear();
        void Save(std::string p_filepath = std::string());
        void Load(std::string p_filepath = std::string());

        void Rename(const std::string& p_newName) override;

        bool Save(WriteStream& p_stream) override;
        bool Load(ReadStream& p_stream)  override;

        bool OnEntityRenamed(const Events::EntityRenamedEvent& p_event);
        bool OnComponentCreated(const Events::ComponentCreatedEvent& p_event);
        bool OnComponentAboutToBeRemoved(const Events::ComponentAboutToBeRemovedEvent& p_event);

        inline  Containers::ThreadSafeList<SceneNode>&  GetEntities()               { return m_nodes;			};
		inline  const SceneNode&						GetRoot()			const   { return *m_head;			};
		inline  const std::string&                      GetFullName()       const   { return m_fullname;        };
		inline  Physics::Scene&                         GetPhysicsScene()           { return m_physicsScene;	};
		inline  const Resources::SkyboxPtr&             GetSkybox()         const   { return m_skybox;	        };


        void AddSystem(System* p_system);

        void AddScriptedSystem(const std::string& p_systemName);
        void RecompileScriptedSystems();

        template<typename T>
        T* GetSystem()
        {
            auto result = m_systems.find(T::SYSTEM_TYPE_ID);
            if (result == m_system.end())
                return nullptr;
            return static_cast<T*>(m_systems.at(T::SYSTEM_TYPE_ID));
        }

        System* GetSystem(const SystemTypeID& p_systemTypeID);

        template<typename T>
        void RemoveSystem()
        {
            auto result = m_systems.find(T::SYSTEM_TYPE_ID);
            if (result != m_system.end())
                m_systems.erase(T::SYSTEM_TYPE_ID);
        }

        void RemoveSystem(System* p_system);
        void RemoveSystem(const SystemTypeID& p_systemTypeID);

        void EnableSystems();
        void DisableSystems();

        void DebugPrint(SceneNode* p_debugHead = nullptr);


        template<class T>
        inline std::unordered_map<ComponentID, IComponent*>::iterator begin()
        {
            return GetComponentContainer<T>().begin();
        }

        template<class T>
        inline std::unordered_map<ComponentID, IComponent*>::iterator end()
        {
            return GetComponentContainer<T>().end();
        }

        ScriptedComponent* FindScriptedComponent(const std::string& p_componentTypeName);

        static void Expose();

    protected:

        virtual void PrepareImpl()  override;
        virtual void LoadImpl()     override;
        virtual void UnloadImpl()   override;

        bool SaveNode(WriteStream& p_stream, SceneNode* p_node);
        bool LoadNode(ReadStream& p_stream, SceneNode* p_node);

        void DebugPrint(std::stringstream& p_stringStream, SceneNode* p_debugHead, int level);

        template<typename T>
        std::unordered_map<ComponentID, IComponent*>& GetComponentContainer()
        {
            ComponentTypeID EID = T::COMPONENT_TYPE_ID;
            auto result = m_componentTypedTables.find(EID);
            if (result != m_componentTypedTables.end())
                return *(result->second);

            m_componentTypedTables[EID] =  std::make_shared<std::unordered_map<ComponentID, IComponent*>>();
            CHAOS_CORE_ASSERT(m_componentTypedTables.find(EID) != m_componentTypedTables.end(), "Scene failed to create ComponentContainer<T>!");
            return *m_componentTypedTables[EID];
        }


    protected:
        static constexpr    uint32_t                        MAX_SCENE_COUNT{ 32 };
        static              Chaos::Memory::PoolAllocator    s_allocator;

        std::string                                         m_fullname;
        std::shared_ptr<SceneNode>                          m_head;
        std::unordered_map<SystemTypeID, System*>           m_systems;
        Containers::ThreadSafeList<SceneNode>               m_nodes;
        Physics::Scene                                      m_physicsScene;
        Resources::SkyboxPtr                                m_skybox        {nullptr};

        std::unordered_map<ComponentTypeID, std::shared_ptr<std::unordered_map<ComponentID, IComponent*>>> m_componentTypedTables;
			SceneRenderParam m_sceneParams;
    };

    using ScenePtr = std::shared_ptr<Scene>;
}