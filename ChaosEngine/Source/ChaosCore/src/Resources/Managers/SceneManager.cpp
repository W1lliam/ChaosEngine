#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Resources/Managers/SceneManager.h"

namespace Chaos
{
    SceneManager::SceneManager(const StringID& p_name) : AResourceManager(p_name, RESOURCE_TYPE, 100)
    {
    }

    SceneManager::~SceneManager()
    {
        RemoveAll();
    }

    AResource* SceneManager::CreateImpl(const std::string& p_name, const StringID& p_group)
    {
        return  new (Scene::s_allocator.Allocate(sizeof(Scene), alignof(Scene))) Scene(this, p_name, p_group);
    }

    void SceneManager::RemoveImpl(AResource* p_resource)
    {
        static_cast<Scene*>(p_resource)->~Scene();
        Scene::s_allocator.Free(p_resource);
    }

    void SceneManager::RemoveAllImpl()
    {
        for (auto& resourcePair : m_resources)
        {
            static_cast<Scene*>(resourcePair.second.get())->~Scene();
        }
        Scene::s_allocator.Clear();
    }

    ScenePtr SceneManager::GetScene(const StringID& p_name, const StringID& p_group)
    {
        return std::static_pointer_cast<Scene>(GetResource(p_name, p_group));
    }
}