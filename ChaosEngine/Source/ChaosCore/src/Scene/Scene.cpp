#include <string>
#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Scene/Scene.h"
#include "ChaosCore/Components/ChildComponent.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"
#include "ChaosCore/Events/SceneEvents.h"
#include "ChaosCore/Events/EventHandler.h"
#include "ChaosCore/Resources/AssetLoader.h"
#include "ChaosCore/Resources/Managers/SkyboxManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
    Memory::PoolAllocator Scene::s_allocator{ sizeof(Scene) * MAX_SCENE_COUNT, sizeof(Scene), alignof(Scene) };

    Scene::Scene(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group) :
        AResource(p_creator, operator""_sid(p_name.c_str(), p_name.size()), p_group),
        m_head{ std::make_shared<SceneNode>() },
        m_physicsScene({ 0.f, -6.f, 0.f }),
        m_fullname(p_name)
    {
        RegisterEventCallback<Events::EntityRenamedEvent>(std::bind(&Scene::OnEntityRenamed, this, std::placeholders::_1));
        RegisterEventCallback<Events::ComponentCreatedEvent>(std::bind(&Scene::OnComponentCreated, this, std::placeholders::_1));
        RegisterEventCallback<Events::ComponentAboutToBeRemovedEvent>(std::bind(&Scene::OnComponentAboutToBeRemoved, this, std::placeholders::_1));
		m_skybox = SkyboxManager::Get().GetCubeMap("ClearSky"_sid);
    }

    Scene::~Scene()
    {
        Clear();
    }

    void Scene::AddEntity(EntityPtr p_entity, EntityPtr p_parent)
    {
        if (p_parent)
        {
            auto foundParent = m_nodes.find_if([&p_parent](SceneNode& p_node)
            {
                return p_node.entity == p_parent;
            });
            if (!foundParent)
            {
                CHAOS_CORE_WARN("Parent {0} not found in scene {0}", p_parent->GetEntityID(), m_name);
            }

            m_nodes.push_front({ p_entity, foundParent.get() });
            foundParent->childs.push_back(m_nodes.front().get());
            p_entity->AddComponent<ChildComponent>(p_parent->GetEntityID());
        }
        else
        {
            m_nodes.push_front({ p_entity, m_head.get() });
            m_head->childs.push_back(m_nodes.front().get());
        }

        std::unordered_map<ComponentTypeID, ComponentID> components =  p_entity->GetAllComponents();
        for (const std::pair<ComponentTypeID, ComponentID>& component : components)
        {
            ComponentTypeID EID = component.first;
            auto result = m_componentTypedTables.find(EID);
            if (result == m_componentTypedTables.end())
            {
                m_componentTypedTables[EID] = std::make_shared<std::unordered_map<ComponentID, IComponent*>>();
            }
                
            CHAOS_CORE_ASSERT(m_componentTypedTables.find(EID) != m_componentTypedTables.end(), "Scene failed to create ComponentContainer<T>!");

            auto foundIt = m_componentTypedTables.at(EID)->find(component.second);
            if (foundIt == m_componentTypedTables.at(EID)->end() &&
                component.first == RigidBodyComponent::COMPONENT_TYPE_ID)
            {
                m_physicsScene.AddActor(reinterpret_cast<RigidBodyComponent*>(ComponentManager::Get().GetComponent(component.second))->Actor());
            }
            (*m_componentTypedTables.at(EID))[component.second] = ComponentManager::Get().GetComponent(component.second);
        }
        Events::EventHandler::Get().Send<Events::EntityCreatedEvent>(*this, p_entity->GetEntityID());
    }

    void Scene::SetParent(const EntityID& p_childID, const EntityID& p_parentID)
    {
        EntityPtr child = EntityManager::Get().GetEntity(p_childID);
        if (!child)
        {
            CHAOS_CORE_WARN("Entity {0} not found", p_childID);
            return;
        }

        EntityPtr parent = EntityManager::Get().GetEntity(p_parentID);
        if (!parent)
        {
            CHAOS_CORE_WARN("Parent {0} not found", p_parentID);
            return;
        }

        auto childNode = m_nodes.find_if([&child](SceneNode& p_node)
        {
            return p_node.entity == child;
        });
        if (!childNode)
        {
            CHAOS_CORE_WARN("Entity {0} not found in scene {1}", child->GetEntityID(), m_name);
            return;
        }
        if (childNode->parent)
        {
            childNode->parent->childs.erase(std::remove(childNode->parent->childs.begin(), childNode->parent->childs.end(), childNode.get()), childNode->parent->childs.end());
        }

        auto parentNode = m_nodes.find_if([&parent](SceneNode& p_node)
        {
            return p_node.entity == parent;
        });
        if (!parentNode)
        {
            CHAOS_CORE_WARN("Parent {0} not found in scene {1}", parent->GetEntityID(), m_name);
            return;
        }
        child->AddComponent<ChildComponent>(parent->GetEntityID());
        childNode->parent = parentNode.get();
        parentNode->childs.emplace_back(childNode.get());
    }

    void Scene::SetParent(EntityPtr p_child, EntityPtr p_parent)
    {
        auto childNode = m_nodes.find_if([&p_child](SceneNode& p_node)
        {
            return p_node.entity == p_child;
        });
        if (!childNode)
        {
            CHAOS_CORE_WARN("Entity {0} not found in scene {1}", p_child->GetEntityID(), m_name);
            return;
        }
        if (childNode->parent)
        {
            childNode->parent->childs.erase(std::remove(childNode->parent->childs.begin(), childNode->parent->childs.end(), childNode.get()), childNode->parent->childs.end());
        }

		std::shared_ptr<SceneNode> parentNode;

		if (p_parent)
		{
			parentNode = m_nodes.find_if([&p_parent](SceneNode& p_node)
			{
				return p_node.entity == p_parent;
			});
			if (!parentNode)
			{
				CHAOS_CORE_WARN("Parent {0} not found in scene {1}", p_parent->GetEntityID(), m_name);
				return;
			}
			p_child->AddComponent<ChildComponent>(p_parent->GetEntityID());
		}
		else
		{
			parentNode = m_head;
		}
        childNode->parent = parentNode.get();
        parentNode->childs.emplace_back(childNode.get());
    }

    void Scene::RemoveEntity(EntityPtr p_entity)
    {
        auto node = m_nodes.find_if([&p_entity](SceneNode& p_node)
        {
            return p_node.entity == p_entity;
        });
        if (!node)
        {
            CHAOS_CORE_WARN("Entity {0} not found in scene {1}", p_entity->GetEntityID(), m_name);
            return;
        }

        if (node->parent)
        {
            node->parent->childs.erase(std::remove(node->parent->childs.begin(), node->parent->childs.end(), node.get()), node->parent->childs.end());
        }

        for (SceneNode* child : node->childs)
        {
            RemoveEntity(child->entity);
        }

        m_nodes.remove_if([&](SceneNode& p_node)
        {
            if (p_node.entity == p_entity)
            {
                std::unordered_map<ComponentTypeID, ComponentID> components = p_node.entity->GetAllComponents();
                for (const std::pair<ComponentTypeID, ComponentID>& component : components)
                {
                    auto result = m_componentTypedTables.find(component.first);
                    if (result != m_componentTypedTables.end())
                    {
                        m_componentTypedTables.at(component.first)->erase(component.second);
                    }
                    if (component.first == RigidBodyComponent::COMPONENT_TYPE_ID)
                        m_physicsScene.DeleteActor(reinterpret_cast<RigidBodyComponent*>(ComponentManager::Get().GetComponent(component.second))->Actor());
                }
                Events::EventHandler::Get().Send<Events::EntityRemovedEvent>(*this, p_node.entity->GetEntityID());
                return true;
            }
            return false;
        });
    }

    bool Scene::OnEntityRenamed(const Events::EntityRenamedEvent& p_event)
    {
        const EntityID& entityID = p_event.GetEntityID();

        auto node = m_nodes.find_if([&entityID](SceneNode& p_node)
        {
            return p_node.entity->GetEntityID() == entityID;
        });
        if (node)
        {
            node->entity->m_entityID = p_event.GetNewEntityID();
        }
        return false;
    }

    bool Scene::OnComponentCreated(const Events::ComponentCreatedEvent& p_event)
    {
        IComponent& component = const_cast<IComponent&>(p_event.GetComponent());
        auto node = m_nodes.find_if([&component](SceneNode& p_node)
        {
            return p_node.entity->GetEntityID() == component.GetOwnerID();
        });
        if (node)
        {
            ComponentTypeID CID = component.GetComponentTypeID();
            auto result = m_componentTypedTables.find(CID);
            if (result == m_componentTypedTables.end())
            {
                m_componentTypedTables[CID] = std::make_shared<std::unordered_map<ComponentID, IComponent*>>();
            }
            CHAOS_CORE_ASSERT(m_componentTypedTables.find(CID) != m_componentTypedTables.end(), "Scene failed to create ComponentContainer<T>!");

            auto foundIt = m_componentTypedTables.at(CID)->find(component.GetComponentID());
            if (foundIt == m_componentTypedTables.at(CID)->end() &&
                component.GetComponentTypeID() == RigidBodyComponent::COMPONENT_TYPE_ID)
            {
                m_physicsScene.AddActor(reinterpret_cast<const RigidBodyComponent&>(component).Actor());
            }
            (*m_componentTypedTables.at(CID))[component.GetComponentID()] =  &component;
        }
        return false;
    }

    bool Scene::OnComponentAboutToBeRemoved(const Events::ComponentAboutToBeRemovedEvent& p_event)
    {
        const IComponent& component = p_event.GetComponent();
        auto node = m_nodes.find_if([&component](SceneNode& p_node)
        {
            return p_node.entity->GetEntityID() == component.GetOwnerID();
        });
        if (node)
        {
            auto result = m_componentTypedTables.find(component.GetComponentTypeID());
            if (result != m_componentTypedTables.end())
                m_componentTypedTables.at(component.GetComponentTypeID())->erase(component.GetComponentID());
            if (component.GetComponentTypeID() == RigidBodyComponent::COMPONENT_TYPE_ID)
                m_physicsScene.DeleteActor(reinterpret_cast<const RigidBodyComponent&>(component).Actor());
        }
        return false;
    }

    ScriptedComponent* Scene::FindScriptedComponent(const std::string& p_componentTypeName)
    {
        const ComponentTypeID CTID = operator""_sid(p_componentTypeName.c_str(), p_componentTypeName.size());

        auto result = m_componentTypedTables.find(CTID);
        if (result != m_componentTypedTables.end())
        {
            return reinterpret_cast<ScriptedComponent*>(result->second->begin()->second);
        }

        return nullptr;
    }

    void Scene::Expose()
    {
        ssq::Class sqScene = ScriptsHandler::Get().GetVirtualMachine().addClass("Scene", ssq::Class::Ctor<Scene(AResourceManager*, std::string, StringID)>());
        sqScene.addFunc("GetName",      &Scene::GetFullName);
        sqScene.addFunc("SetEntityParent", [](Scene* self, AEntity* p_child, AEntity* p_parent)
        {
            self->SetParent(p_child->GetEntityID(), p_parent->GetEntityID());
        });
        sqScene.addFunc("FindComponentOfType", [&](Scene* self, std::string p_component) -> ssq::Instance
        {
            ScriptedComponent* script = self->FindScriptedComponent(p_component);
            if(script)
                return script->GetSqInstance();

            return nullptr;
        });
    }

    void Scene::PrepareImpl()
    {
    }

    void Scene::LoadImpl()
    {
    }

    void Scene::UnloadImpl()
    {
        DisableSystems();
    }

    void Scene::Clear()
    {
        m_nodes.clear();
        m_head->childs.clear();
        m_componentTypedTables.clear();
        m_systems.clear();
    }

    void Scene::Save(std::string p_filepath)
    {
        
        if (p_filepath.empty())
        {
            p_filepath = "Assets/Scenes/";
        }

        if (!std::filesystem::is_directory(p_filepath))
            std::filesystem::create_directory(p_filepath);

        std::ofstream dataStream(p_filepath + m_fullname + CHAOS_SCENE_EXTENSION, std::ios::out | std::ios::binary);

        if (dataStream.is_open())
        {
            CHAOS_CORE_TRACE("Saving Scene {}", m_fullname);
            ResourceHeader header(E_ASSETTYPE_SCENE);
            WriteStream writeStream(1024);

            header.Serialize(writeStream);
            Save(writeStream);
            writeStream.Finish();

            uint32_t fileSize = writeStream.GetBytesProcessed();

            memcpy(((char*)&writeStream.GetBuffer()[0]) + header.resourceSizeOffset, &fileSize, sizeof(fileSize));
            dataStream.write((char*)&writeStream.GetBuffer()[0], writeStream.GetBytesProcessed());
        }
        else
        {
            CHAOS_CORE_ERROR("Cannot open scene file!");
        }
        dataStream.close();
    }
    void Scene::Load(std::string p_filepath)
    {
        if (p_filepath.empty())
        {
            p_filepath = "Assets/Scenes/" + m_fullname + CHAOS_SCENE_EXTENSION;
        }

        std::ifstream dataStream(p_filepath, std::ios::in | std::ios::binary | std::ios::ate);

        if (dataStream.is_open())
        {
            std::streamsize size = dataStream.tellg();
            dataStream.seekg(0, std::ios::beg);

            std::vector<uint8_t> buffer(size);
            dataStream.read((char*)buffer.data(), size);
            dataStream.close();

            ReadStream readStream(buffer.data(), (uint32_t)size);
            ResourceHeader header;
            header.Serialize(readStream);
            if (header.signature == ".PNS" && header.resourceType == E_ASSETTYPE_SCENE && header.resourceSize == size)
            {
                CHAOS_CORE_TRACE("Loading Scene {}", m_fullname);
                m_nodes.for_each([&](SceneNode& p_node)
                {
                    EntityManager::Get().DestroyEntity(p_node.entity->GetEntityID());
                });
                Clear();
                Load(readStream);
            }
            else
            {
                CHAOS_CORE_WARN("Scene Asset is invalid!");
            }
            readStream.Finish();

            CHAOS_CORE_TRACE("FILE SIZE: {0}", header.resourceSize);
        }
        else
        {
            CHAOS_CORE_ERROR("Cannot open scene file!");
            dataStream.close();
        }
    }

	void Scene::Rename(const std::string& p_newName)
	{
        if (m_fullname == p_newName)
            return;
        if (filesystem::exists("Assets/" + m_fullname + ".resdecl"))
        {
            std::ifstream readFile("Assets/" + m_fullname + ".resdecl", std::ios::in);
            if (readFile.is_open())
            {
                std::string content(static_cast<std::stringstream const&>(std::stringstream() << readFile.rdbuf()).str());
            
                //Search & Replace
                auto pos = content.find(m_fullname);
                while (pos != std::string::npos) {
                    content.replace(pos, m_fullname.length(), p_newName);
                    pos = content.find(m_fullname, pos);
                }

                readFile.close();
                std::ofstream writeFile("Assets/" + m_fullname + ".resdecl", std::ios::out | std::ios::trunc);
                if (writeFile.is_open())
                {
                    writeFile << content;
                }
                writeFile.close();
            }
            else
            {
                readFile.close();
            }
            filesystem::rename("Assets/" + m_fullname + ".resdecl", "Assets/" + p_newName + ".resdecl");
        }
		m_fullname = p_newName;
		AResource::Rename(p_newName);
	}

    bool Scene::Save(WriteStream & p_stream)
    {
        SerializeString(p_stream, m_fullname);

        uint32_t systemCount = static_cast<uint32_t>(m_systems.size());
        SerializeUnsignedInt(p_stream, systemCount, 0, UINT32_MAX);

        for(const std::pair<SystemTypeID, System*>& p_systemPair : m_systems)
        {
            SystemTypeID STID = p_systemPair.first;
            std::string systemName = p_systemPair.second->GetSystemTypeName();
            SerializeStringID(p_stream, STID);
            SerializeString(p_stream, systemName);

            p_systemPair.second->Save(p_stream);
        };

        uint32_t childCount = (uint32_t)m_head->childs.size();
        SerializeUnsignedInt(p_stream, childCount, 0, UINT32_MAX);

        for(SceneNode* child : m_head->childs)
            SaveNode(p_stream, child);

        SerializeStringID(p_stream, const_cast<StringID&>(m_skybox->GetName()));
        SerializeStringID(p_stream, const_cast<StringID&>(m_skybox->GetGroup()));

        SerializeUnsignedInt(p_stream, m_sceneParams.bloomBlurPasses, 0u, 30u);
        SerializeFloat(p_stream, m_sceneParams.bloomThreshold);
        SerializeFloat(p_stream, m_sceneParams.bloomIntesity);
        SerializeFloat(p_stream, m_sceneParams.gamma);
        SerializeFloat(p_stream, m_sceneParams.skyboxExposure);
        return true;
    }
    bool Scene::Load(ReadStream& p_stream)
    {
        SerializeString(p_stream, m_fullname);
        m_name = operator""_sid(m_fullname.c_str(), m_fullname.size());

        uint32_t systemCount;
        SerializeUnsignedInt(p_stream, systemCount, 0, UINT32_MAX);

        while (systemCount--)
        {
            SystemTypeID STID;
            std::string systemName;
            SerializeStringID(p_stream, STID);
            SerializeString(p_stream, systemName);
            System& system = SystemManager::Get().LoadSystem(p_stream, STID, systemName);
            m_systems[system.GetSystemTypeID()] = &system;
        }

        uint32_t childCount;
        SerializeUnsignedInt(p_stream, childCount, 0, UINT32_MAX);
        m_head->childs.reserve(childCount);

        while(childCount--)
        {
            m_nodes.push_front({m_head.get()});
            m_head->childs.push_back(m_nodes.front().get());
            LoadNode(p_stream, m_head->childs.back());
        }

        StringID skyboxID;
        StringID skyboxGroupID;
        SerializeStringID(p_stream, skyboxID);
        SerializeStringID(p_stream, skyboxGroupID);

        m_skybox = SkyboxManager::Get().GetCubeMap(skyboxID, skyboxGroupID);
		SerializeUnsignedInt(p_stream, m_sceneParams.bloomBlurPasses, 0u, 30u);
		SerializeFloat(p_stream, m_sceneParams.bloomThreshold);
		SerializeFloat(p_stream, m_sceneParams.bloomIntesity);
		SerializeFloat(p_stream, m_sceneParams.gamma);
		SerializeFloat(p_stream, m_sceneParams.skyboxExposure);
        return true;
    }
    
    bool Scene::SaveNode(WriteStream& p_stream, SceneNode* p_node)
    {
        if (p_node->parent && p_node->parent->entity)
        {
            SerializeStringID(p_stream, const_cast<StringID&>(p_node->parent->entity->GetEntityID()));
        }
        CHAOS_CORE_ASSERT(p_node->entity, "Node missing entity");
        SerializeStringID(p_stream, const_cast<StringID&>(p_node->entity->GetEntityID()));

        SerializeObject(p_stream, *p_node->entity);
        
        uint32_t childCount = (uint32_t)p_node->childs.size();
        SerializeUnsignedInt(p_stream, childCount, 0, UINT32_MAX);

        for (SceneNode* child : p_node->childs)
            SaveNode(p_stream, child);
        return true;
    }
    bool Scene::LoadNode(ReadStream& p_stream, SceneNode* p_node)
    {
        if (p_node->parent && p_node->parent->entity)
        {
            StringID parentID;
            SerializeStringID(p_stream, parentID);
            CHAOS_CORE_ASSERT(p_node->parent->entity->GetEntityID() == parentID, "Wrong Parent: expected {0}, found {1}", parentID, p_node->parent->entity->GetEntityID());
        }

        StringID entityID;
        SerializeStringID(p_stream, const_cast<StringID&>(entityID));

        p_node->entity = EntityManager::Get().GetEntity(entityID);
        if(!p_node->entity)
            p_node->entity = EntityManager::Get().CreateEntity<AEntity>(entityID);
        SerializeObject(p_stream, *p_node->entity);

        uint32_t childCount = (uint32_t)p_node->childs.size();
        SerializeUnsignedInt(p_stream, childCount, 0, UINT32_MAX);
        p_node->childs.reserve(childCount);

        while (childCount--)
        {
            m_nodes.push_front({p_node});
            p_node->childs.push_back(m_nodes.front().get());
            LoadNode(p_stream, p_node->childs.back());
        }
        return true;
    }

    void Scene::AddSystem(System* p_system)
    {
        m_systems[p_system->GetSystemTypeID()] = p_system;
    }

    void Scene::AddScriptedSystem(const std::string& p_systemName)
    {
        ScriptedSystem* system = ScriptsHandler::Get().CompileSystem(p_systemName);
        m_systems[system->GetSystemTypeID()] = system;
    }

    void Scene::RecompileScriptedSystems()
    {
        std::vector<ScriptedSystem*> updatedSystems{};

       for(const std::pair<SystemTypeID, System*>& p_pair : m_systems)
        {
            ScriptedSystem* scriptSystem = dynamic_cast<Chaos::ScriptedSystem*>(p_pair.second);
            if (scriptSystem)
            {
                updatedSystems.push_back(scriptSystem);
            }
        };
       for (ScriptedSystem* system : updatedSystems)
       {
           RemoveSystem(system);
           AddScriptedSystem(system->GetSystemTypeName());
       }
    }

    System* Scene::GetSystem(const SystemTypeID & p_systemTypeID)
    {
        auto result = m_systems.find(p_systemTypeID);
        if (result == m_systems.end())
            return nullptr;
        return m_systems.at(p_systemTypeID);
    }

    void Scene::RemoveSystem(System* p_system)
    {
        auto result = m_systems.find(p_system->GetSystemTypeID());
        if (result != m_systems.end())
            m_systems.erase(p_system->GetSystemTypeID());
    }

    void Scene::RemoveSystem(const SystemTypeID& p_systemTypeID)
    {

        auto result = m_systems.find(p_systemTypeID);
        if (result == m_systems.end())
            m_systems.erase(p_systemTypeID);
    }

    void Scene::EnableSystems()
    {
        for (const std::pair<SystemTypeID, System*>& p_systemPair : m_systems)
        {
            p_systemPair.second->Enable();   
        };
    }

    void Scene::DisableSystems()
    {
        for (const std::pair<SystemTypeID, System*>& p_systemPair : m_systems)
        {
            p_systemPair.second->Disable();  
        };
    }

    void Scene::DebugPrint(SceneNode* p_debugHead)
    {
        std::stringstream ss;
        if(!p_debugHead)
            p_debugHead = m_head.get();

        ss << "\n" << m_fullname << "\n";
        ss << "\n" << m_name << "\n";

        if (!p_debugHead->childs.empty())
        {
            ss << "|\n";
        }

        for (auto child : p_debugHead->childs)
        {
            DebugPrint(ss, child, 1);
            ss << "|\n";
        }
        CHAOS_CORE_FATAL("{}", ss.str());
    }
    void Scene::DebugPrint(std::stringstream& p_stringStream, SceneNode* p_node, int p_level)
    {
        p_stringStream << "|";
        for (int i = 0; i < p_level - 1; ++i)
            p_stringStream << "        ";
        p_stringStream << "--------" << p_node->entity->GetEntityID() << "\n";
        ++p_level;


        for (size_t i = 0; i < p_node->childs.size(); ++i)
        {
            p_stringStream << "|";
            for (int i = 0; i < p_level - 1; ++i)
                p_stringStream << "        ";
            p_stringStream << "|\n";
            DebugPrint(p_stringStream, p_node->childs[i], p_level);
        }
    }

}