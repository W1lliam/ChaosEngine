#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"

namespace Chaos
{
    AResourceManager::AResourceManager(const StringID& p_name, AssetType p_resourceType, uint32_t p_loadOrder) : m_name{p_name}, m_resourceType{p_resourceType}, m_loadOrder{p_loadOrder}
    {
        ResourceGroupManager::Get().RegisterResourceManager(m_resourceType, this);
    }

    AResourceManager::~AResourceManager()
    {
        ResourceGroupManager::Get().UnregisterResourceManager(m_resourceType);
    }

    ResourcePtr AResourceManager::CreateResource(const std::string& p_name, const StringID& p_group)
    {
        ResourcePtr resource(CreateImpl(p_name, p_group), std::bind(&AResourceManager::RemoveImpl, this, std::placeholders::_1));
        AddImpl(resource);
        ResourceGroupManager::Get()._NotifyResourceCreated(resource);
        return resource;

    }

    std::pair<ResourcePtr, bool>  AResourceManager::CreateOrRetrieve(const std::string& p_name, const StringID& p_group)
    {
        ResourcePtr resource = GetResource(operator""_sid(p_name.c_str(), p_name.size()), p_group);
        bool created = false;
        if (!resource)
        {
            created = true;
            resource = CreateResource(p_name, p_group);
        }

        return std::pair<ResourcePtr, bool>(resource, created);
    }

    ResourcePtr AResourceManager::Prepare(const std::string& p_name, const StringID & p_group)
    {
        ResourcePtr resource = CreateOrRetrieve(p_name, p_group).first;

        resource->Prepare();
        return resource;
    }

    ResourcePtr AResourceManager::Load(const std::string& p_name, const StringID & p_group)
    {
        ResourcePtr resource = CreateOrRetrieve(p_name, p_group).first;

        resource->Load();
        return resource;
    }

    void AResourceManager::ReloadAll()
    {
        for (auto resourceIt : m_resources)
        {
            resourceIt.second->Reload();
        }
    }

    void AResourceManager::ReloadUnreferenced()
    {
        for (auto& resourceIt : m_resources)
        {
            // A use count of 2 means that only RGM and RM have references
            if (resourceIt.second.use_count() == ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS)
            {
                AResource* resource = resourceIt.second.get();
                resource->Reload();
            }
        }
    }

    void AResourceManager::Unload(const StringID& p_name)
    {
        ResourcePtr resource = GetResource(p_name);

        if(resource)
        {
            resource->Unload();
        }
    }

    void AResourceManager::Unload(ResourcePtr& p_resource)
    {
        if (p_resource)
        {
            p_resource->Unload();
        }
    }

    void AResourceManager::UnloadAll()
    {
        for (auto& resourceIt : m_resources)
        {
            resourceIt.second->Unload();
        }
    }

    void AResourceManager::UnloadUnreferenced()
    {
        for (auto& resourceIt : m_resources)
        {
            if (resourceIt.second.use_count() == ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS)
            {
                resourceIt.second->Unload();
            }
        }
    }

    void AResourceManager::Remove(const StringID& p_name)
    {
        ResourcePtr resource = GetResource(p_name);

        if (resource)
        {
            Remove(resource);
        }
    }

    void AResourceManager::Remove(ResourcePtr& p_resource)
    {
        if (ResourceGroupManager::Get().IsResourceGroupInGlobalPool(p_resource->GetGroup()))
        {
            ResourceMap::iterator nameIt = m_resources.find(p_resource->GetName());
            if (nameIt != m_resources.end())
            {
                m_resources.erase(nameIt);
            }
        }
        else
        {
            GroupedResourceMap::iterator groupIt = m_groupedResources.find(p_resource->GetGroup());
            if (groupIt != m_groupedResources.end())
            {
                ResourceMap::iterator nameIt = groupIt->second.find(p_resource->GetName());
                if (nameIt != groupIt->second.end())
                {
                    groupIt->second.erase(nameIt);
                }

                if (groupIt->second.empty())
                {
                    m_groupedResources.erase(groupIt);
                }
            }
        }
        ResourceGroupManager::Get()._NotifyResourceRemoved(p_resource);
    }

    void AResourceManager::RemoveAll()
    {
            m_resources.clear();
            m_groupedResources.clear();

            ResourceGroupManager::Get()._NotifyAllResourcesRemoved(this);
            RemoveAllImpl();
    }

    void AResourceManager::RemoveUnreferenced()
    {
        for (ResourceMap::iterator i = m_resources.begin(); i != m_resources.end(); )
        {
            // A use count of 2 means that only RGM and RM have references
            if (i->second.use_count() == ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS)
            {
                AResource* resource = (i++)->second.get();
                Remove(resource->GetName());
            }
            else
            {
                ++i;
            }
        }
    }

    ResourcePtr AResourceManager::GetResource(const StringID& p_name, const StringID& p_group)
    {
        ResourcePtr resource;
            GroupedResourceMap::iterator groupIt = m_groupedResources.find(p_group);

            if (groupIt != m_groupedResources.end())
            {
                ResourceMap::iterator it = groupIt->second.find(p_name);

                if (it != groupIt->second.end())
                {
                    resource = it->second;
                }
            }

        if (!resource)
        {
            ResourceMap::iterator it = m_resources.find(p_name);

            if (it != m_resources.end())
            {
                resource = it->second;
            }
            else
            {
                for (auto group : m_groupedResources)
                {
                    ResourceMap::iterator resourceIt = group.second.find(p_name);

                    if (resourceIt != group.second.end())
                    {
                        resource = resourceIt->second;
                        break;
                    }
                }
            }
        }
        return resource;
    }

    void AResourceManager::AddImpl(ResourcePtr& p_resource)
    {
        std::pair<ResourceMap::iterator, bool> result;
        if (ResourceGroupManager::Get().IsResourceGroupInGlobalPool(p_resource->GetGroup()))
        {
            result = m_resources.insert(ResourceMap::value_type(p_resource->GetName(), p_resource));
        }
        else
        {
            GroupedResourceMap::iterator groupIt = m_groupedResources.find(p_resource->GetGroup());
            if (groupIt == m_groupedResources.end())
            {
                groupIt = m_groupedResources.emplace(p_resource->GetGroup(), ResourceMap{}).first;
            }
            result = groupIt->second.insert(ResourceMap::value_type(p_resource->GetName(), p_resource));
        }

        if (!result.second)
        {
            CHAOS_CORE_WARN("Resource with the name {0} already exists",  p_resource->GetName());
        }
    }
}