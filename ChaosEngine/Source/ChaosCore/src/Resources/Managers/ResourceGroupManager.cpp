#include "ChaosCore/chaos_core_pch.h"
#include "ChaosDebug.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Events/EventHandler.h"

namespace Chaos
{
    const StringID ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME {"DefaultResources"_sid};

    ResourceGroupManager::ResourceGroupManager()
    {
        CreateResourceGroup(DEFAULT_RESOURCE_GROUP_NAME);
    }

    ResourceGroupManager::~ResourceGroupManager()
    {
        m_resourceGroupMap.clear();
        m_resourceGroupAllocator.Clear();
    }

    void ResourceGroupManager::CreateResourceGroup(const StringID& p_group, const bool p_inGlobalPool)
    {
        CHAOS_CORE_TRACE("Creating resource group {0}", p_group);

        void* groupMem = m_resourceGroupAllocator.Allocate(sizeof(ResourceGroup), alignof(ResourceGroup));
        if (GetResourceGroup(p_group))
        {
            throw std::exception((std::string("Resource group with name " + (p_group)) + " already exists!").c_str());
        }

        if (!groupMem)
        {
            CHAOS_CORE_ERROR("Resource Group Allocation failed!");
            return;
        }
        ResourceGroup* group = new (groupMem) ResourceGroup{ p_group, ResourceGroup::E_Status::UNINITIALIZED, p_inGlobalPool };
        m_resourceGroupMap.insert(ResourceGroupMap::value_type(p_group, group));
    }

    void ResourceGroupManager::AddResourceLocation(const std::string& p_location, const StringID& p_group, bool p_recursive, bool p_readOnly)
    {
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            CreateResourceGroup(p_group);
            group = GetResourceGroup(p_group);
        }
        if (!filesystem::exists(p_location))
        {
            CHAOS_CORE_WARN("Resource location {0} not found", p_location);
            return;
        }

        group->locationList.emplace_back(p_location, p_recursive, p_readOnly);
        if (p_recursive)
        {
            for (auto file : filesystem::recursive_directory_iterator(p_location))
            {
                group->resourceIndex[operator""_sid(file.path().stem().u8string().c_str(), file.path().stem().u8string().length())] = file.path();
            }
        }
        else
        {
            for (auto file : filesystem::directory_iterator(p_location))
            {
                group->resourceIndex[operator""_sid(file.path().stem().u8string().c_str(), file.path().stem().u8string().length())] = file.path();
            }
        }

        std::stringstream log;
        log << "Added resource location " << p_location << " to resource group " << p_group;
        if (p_recursive)
            log << " with recursive option";
        CHAOS_CORE_TRACE(log.str());
    }

    void ResourceGroupManager::RemoveResourceLocation(const std::string& p_location, const StringID & p_group)
    {
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }

        for (auto& location : group->locationList)
        {
            if (location.folder == p_location)
            {
                ResourceLocationIndex::iterator locationIndexIt = group->resourceIndex.find(operator""_sid(p_location.c_str(), p_location.size()));
                if (locationIndexIt != group->resourceIndex.end() && locationIndexIt->second == location.folder)
                    group->resourceIndex.erase(locationIndexIt);
                break;
            }
        }
        CHAOS_CORE_TRACE("Removed resource location {0}", p_location);
    }

    void ResourceGroupManager::DeclareResource(const std::string& p_name, AssetType p_resourceType, const StringID & p_group)
    {
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }
        group->resourceDeclarations.emplace_back(p_name, p_resourceType);
    }

    void ResourceGroupManager::UndeclareResource(const std::string& p_name, const StringID & p_group)
    {
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }

        for (auto declarationIt = group->resourceDeclarations.begin(); declarationIt != group->resourceDeclarations.end(); ++declarationIt)
        {
            if (declarationIt->resourceName == p_name)
            {
                group->resourceDeclarations.erase(declarationIt);
                break;
            }
        }
    }

    void ResourceGroupManager::LoadResourceDeclaration(const std::string & p_path)
    {
        std::ifstream stream(p_path);
        if (stream.is_open())
        {
            std::string line;
            std::getline(stream, line);
            if (line == "#Groups")
            {
                while (std::getline(stream, line) && line[0] != '#')
                {
                    if (line.empty())
                        continue;

                    StringID groupID = operator""_sid(line.c_str(), line.size());
                    if (!GetResourceGroup(groupID))
                    {
                        CreateResourceGroup(groupID);
                    }

                }
            }

            if (line == "#Locations")
            {
                while (std::getline(stream, line) && line[0] != '#')
                {
                    if (line.empty())
                        continue;

                    std::stringstream  lineStream(line);
                    std::string        location;
                    std::getline(lineStream, location, ',');

                    std::string        groupString;
                    std::getline(lineStream, groupString, ',');

                    std::string        recursive;
                    std::getline(lineStream, recursive);

                    StringID groupID = operator""_sid(groupString.c_str(), groupString.size());

                    ResourceGroup* group = GetResourceGroup(groupID);
                    if (!group)
                    {
                        CreateResourceGroup(groupID);
                        group = GetResourceGroup(groupID);
                    }

                    if(recursive == "true")
                        AddResourceLocation(filesystem::path(p_path).remove_filename().string() + location, group->name, true);
                    else
                        AddResourceLocation(filesystem::path(p_path).remove_filename().string() + location, group->name, false);
                }
            }

            if (line == "#Resources")
            {
                while (std::getline(stream, line) && line[0] != '#')
                {
                    std::stringstream  lineStream(line);
                    std::string        name;
                    std::getline(lineStream, name, ',');

                    std::string        type;
                    std::getline(lineStream, type, ',');

                    std::string        groupString;
                    std::getline(lineStream, groupString);
                    StringID groupID = operator""_sid(groupString.c_str(), groupString.size());

                    ResourceGroup* group = GetResourceGroup(groupID);
                    if (!group)
                    {
                        CreateResourceGroup(groupID);
                        group = GetResourceGroup(groupID);
                    }

                    group->resourceDeclarations.remove_if([&name](const ResourceDeclaration& p_decl) { return p_decl.resourceName == name; });
                    if (type == "MESH")
                        group->resourceDeclarations.emplace_back(name, E_ASSETTYPE_MESH);
                    else if (type == "TEXTURE")
                        group->resourceDeclarations.emplace_back(name, E_ASSETTYPE_TEXTURE);
                    else if (type == "SKYBOX")
                        group->resourceDeclarations.emplace_back(name, E_ASSETTYPE_SKYBOX);
                    else if (type == "SHADER")
                        group->resourceDeclarations.emplace_back(name, E_ASSETTYPE_SHADER);
                    else if (type == "MATERIAL")
                        group->resourceDeclarations.emplace_back(name, E_ASSETTYPE_MATERIAL);
					else if (type == "AUDIO")
						group->resourceDeclarations.emplace_back(name, E_ASSETTYPE_AUDIO);
					else if (type == "SKELETALMESH")
						group->resourceDeclarations.emplace_back(name, E_ASSETTYPE_SKELETALMESH);
                }
            }
        }
        else
        {
            CHAOS_CORE_WARN("Could not open resource declaration at {0}", p_path);
        }
        stream.close();
    }

    void ResourceGroupManager::InitializeResourceGroup(const StringID& p_group)
    {
        CHAOS_CORE_TRACE("Initializing resource group {0}", p_group);
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }

        if (group->status == ResourceGroup::E_Status::UNINITIALIZED)
        {
            group->status = ResourceGroup::E_Status::INITIALIZING;
            m_currentGroup = group;
            CHAOS_CORE_TRACE("Creating resources for group {0}", group->name);
            CreateDeclaredResources(group);
            group->status = ResourceGroup::E_Status::INITIALIZED;
            CHAOS_CORE_TRACE("All Done");
            m_currentGroup = nullptr;
        }
    }

    void ResourceGroupManager::InitializeAllResourceGroups()
    {
        CHAOS_CORE_TRACE("Initializing all resource groups");
        ResourceGroupMap::iterator it, itEnd;
        itEnd = m_resourceGroupMap.end();
        for (it = m_resourceGroupMap.begin(); it != itEnd; it++)
        {
            ResourceGroup* group = it->second;
            if (group->status == ResourceGroup::E_Status::UNINITIALIZED)
            {
                group->status = ResourceGroup::E_Status::INITIALIZING;
                m_currentGroup = group;
                CHAOS_CORE_TRACE("Creating resources for group {0}", group->name);
                CreateDeclaredResources(group);
                group->status = ResourceGroup::E_Status::INITIALIZED;
                CHAOS_CORE_TRACE("All Done");
                m_currentGroup = nullptr;
            }
        }
    }

    void ResourceGroupManager::PrepareResourceGroup(const StringID& p_group, bool p_prepareMainResources)
    {
        CHAOS_CORE_TRACE("Preparing resource group {0} - Resources: {1}", p_group, p_prepareMainResources);
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }

        m_currentGroup = group;
        if (p_prepareMainResources)
        {
            for (auto resourceSet : group->loadResourceOrderMap)
            {
                size_t loadedResourceCount = 0;
                LoadUnloadResourceSet::iterator listIt = resourceSet.second.begin();
                while (listIt != resourceSet.second.end())
                {
                    ResourcePtr resource = *listIt;
                    resource->Prepare();
                    ++loadedResourceCount;

                    if (resource->GetGroup() != p_group)
                    {
                        listIt = resourceSet.second.begin();
                        std::advance(listIt, loadedResourceCount);
                    }
                    else
                    {
                        ++listIt;
                    }
                }
            }
        }

        m_currentGroup = nullptr;
    }

    void ResourceGroupManager::LoadAllResourceGroups()
    {
        CHAOS_CORE_TRACE("Loading all resource groups");
        ResourceGroupMap::iterator it, itEnd;
        itEnd = m_resourceGroupMap.end();
        for (it = m_resourceGroupMap.begin(); it != itEnd; it++)
        {
            ResourceGroup* group = it->second;
            if (group->status == ResourceGroup::E_Status::INITIALIZED)
            {
                group->status = ResourceGroup::E_Status::LOADING;
                m_currentGroup = group;
                for (auto resourceSet : group->loadResourceOrderMap)
                {
                    size_t loadedResourceCount = 0;
                    LoadUnloadResourceSet::iterator listIt = resourceSet.second.begin();
                    while (listIt != resourceSet.second.end())
                    {
                        ResourcePtr resource = *listIt;

                        resource->Load();
                        ++loadedResourceCount;

                        if (resource->GetGroup() != group->name)
                        {
                            listIt = resourceSet.second.begin();
                            std::advance(listIt, loadedResourceCount);
                        }
                        else
                        {
                            ++listIt;
                        }
                    }
                }
                m_currentGroup = nullptr;
            }
        }
    }

    void ResourceGroupManager::LoadResourceGroup(const StringID& p_group, bool p_loadMainResources)
    {
        CHAOS_CORE_TRACE("Loading resource group {0} - Resources: {1}", p_group, p_loadMainResources);
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }

        m_currentGroup = group;

        if (p_loadMainResources)
        {
            group->status = ResourceGroup::E_Status::LOADING;
            for (auto resourceSet : group->loadResourceOrderMap)
            {
                size_t loadedResourceCount = 0;
                LoadUnloadResourceSet::iterator listIt = resourceSet.second.begin();
                while (listIt != resourceSet.second.end())
                {
                    ResourcePtr resource = *listIt;

                    resource->Load();
                    ++loadedResourceCount;

                    if (resource->GetGroup() != p_group)
                    {
                        listIt = resourceSet.second.begin();
                        std::advance(listIt, loadedResourceCount);
                    }
                    else
                    {
                        ++listIt;
                    }
                }
            }
        }

        group->status = ResourceGroup::E_Status::LOADED;
        m_currentGroup = nullptr;
        CHAOS_CORE_TRACE("Finished loading resource group {0}", p_group);
    }

    void ResourceGroupManager::UnloadResourceGroup(const StringID& p_group, bool p_reloadableOnly)
    {
        CHAOS_CORE_TRACE("Unloading resource group {0}", p_group);
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }

        m_currentGroup = group;
        for (auto orderIt = group->loadResourceOrderMap.rbegin(); orderIt != group->loadResourceOrderMap.rend(); ++orderIt)
        {
            for (auto resourceIt = orderIt->second.begin(); resourceIt != orderIt->second.end(); ++resourceIt)
            {
               (*resourceIt)->Unload();
            }
        }

        group->status = ResourceGroup::E_Status::INITIALIZED;
        m_currentGroup = nullptr;

        CHAOS_CORE_TRACE("Finished unloading resource group {0}", p_group);
    }

    void ResourceGroupManager::ClearResourceGroup(const StringID& p_group)
    {
        CHAOS_CORE_TRACE("Clearing resource group {0}", p_group);
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            CHAOS_CORE_WARN("Cannot find group with name {}", p_group);
            return;
        }

        m_currentGroup = group;
        DropGroupContents(group);
        group->status = ResourceGroup::E_Status::UNINITIALIZED;
        m_currentGroup = nullptr;
        CHAOS_CORE_TRACE("Finished clearing resource group {0}", p_group);
    }

    void ResourceGroupManager::DestroyResourceGroup(const StringID& p_group)
    {
        CHAOS_CORE_TRACE("Destroying resource group {0}", p_group);
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }

        m_currentGroup = group;
        UnloadResourceGroup(p_group, false);
        DropGroupContents(group);
        DeleteGroup(group);
        m_resourceGroupMap.erase(m_resourceGroupMap.find(p_group));
        m_currentGroup = nullptr;;
    }

    bool ResourceGroupManager::IsResourceGroupInGlobalPool(const StringID& p_group) const
    {
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }
        return group->inGlobalPool;
    }

    void ResourceGroupManager::RegisterResourceManager(AssetType p_resourceType, AResourceManager* p_resourceManager)
    {
        CHAOS_CORE_TRACE("Registering {0}", p_resourceManager->GetName());
        m_resourceManagerMap[p_resourceType] = p_resourceManager;
    }

    void ResourceGroupManager::UnregisterResourceManager(AssetType p_resourceType)
    {
        ResourceManagerMap::iterator it = m_resourceManagerMap.find(p_resourceType);
        if (it != m_resourceManagerMap.end())
        {
            CHAOS_CORE_TRACE("Unregistering {0}", it->second->GetName());
            m_resourceManagerMap.erase(it);
        }
    }

    std::ifstream ResourceGroupManager::OpenResource(const StringID& p_resource, const StringID& p_group, int p_mode)
    {
        CHAOS_CORE_ASSERT(p_resource != StringID(), "Resource name is empty");
        ResourceGroup* group = GetResourceGroup(p_group);
        if (!group)
        {
            throw std::exception("Cannot find group with name " + p_group);
        }

        auto it = group->resourceIndex.find(p_resource);
        if (it != group->resourceIndex.end())
        {
            if(p_mode & std::ios::in)
                return std::ifstream(it->second, p_mode);

        }
        else
        {
            for (auto& location : group->locationList)
            {
                if (filesystem::exists(location.folder/p_resource.string))
                {
                    if (p_mode & std::ios::in)
                        return  std::ifstream(location.folder/p_resource.string, p_mode);
                }
            }
        }
        throw std::exception("Resource {0} not found", p_resource);
    }

	std::string ResourceGroupManager::GetResourcePath(const StringID & p_resource, const StringID & p_group)
	{
		CHAOS_CORE_ASSERT(p_resource != StringID(), "Resource name is empty");
		ResourceGroup* group = GetResourceGroup(p_group);
		if (!group)
		{
            CHAOS_CORE_WARN("Cannot find group with name {}", p_group);
            return std::string();
		}

		auto it = group->resourceIndex.find(p_resource);
		if (it != group->resourceIndex.end())
		{
			return it->second.string();
		}
		else
		{
			for (auto& location : group->locationList)
			{
				if (filesystem::exists(location.folder / p_resource.string))
				{
					return  (location.folder / p_resource.string).string();
				}
			}
		}
        CHAOS_CORE_WARN("Resource location {0} not found", p_resource);
        return std::string();
	}

    ResourceGroupManager::ResourceGroup* ResourceGroupManager::GetResourceGroup(const StringID& p_group) const
    {
        auto it = m_resourceGroupMap.find(p_group);
        if (it != m_resourceGroupMap.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void ResourceGroupManager::CreateDeclaredResources(ResourceGroup* p_group)
    {
        for (auto& declaration : p_group->resourceDeclarations)
        {
            auto fileIt = p_group->resourceIndex.find(operator""_sid(declaration.resourceName.c_str(), declaration.resourceName.size()));
            if (fileIt == p_group->resourceIndex.end())
            {
                CHAOS_CORE_WARN("Resource {0} not found in added locations", declaration.resourceName);
                continue;
            }
            AResourceManager& manager = GetResourceManager(declaration.resourceType);
            ResourcePtr resource = manager.CreateResource(declaration.resourceName, p_group->name);

            if (p_group->loadResourceOrderMap.find(manager.GetLoadingOrder()) == p_group->loadResourceOrderMap.end())
            {
                p_group->loadResourceOrderMap.emplace(manager.GetLoadingOrder(), LoadUnloadResourceSet{});
            }
            p_group->loadResourceOrderMap[manager.GetLoadingOrder()].emplace(resource);
        }
    }

    void ResourceGroupManager::DropGroupContents(ResourceGroup* p_group)
    {
        bool setGroup = false;
        if (!m_currentGroup)
        {
            m_currentGroup = p_group;
            setGroup = true;
        }

        for (auto& resourceSet : p_group->loadResourceOrderMap)
        {
            for (auto& resource : resourceSet.second)
            {
                resource->GetCreator()->Remove(resource->GetName());
            }
        }
        p_group->loadResourceOrderMap.clear();

        if (setGroup)
        {
            m_currentGroup = nullptr;
        }
    }

    void ResourceGroupManager::DeleteGroup(ResourceGroup* p_group)
    {
        m_resourceGroupAllocator.Free(p_group);
        p_group = nullptr;
    }

    AResourceManager& ResourceGroupManager::GetResourceManager(AssetType p_resourceType)
    {
        ResourceManagerMap::iterator i = m_resourceManagerMap.find(p_resourceType);
        if (i == m_resourceManagerMap.end())
        {
            throw std::out_of_range("Cannot locate resource manager for resource type: " + p_resourceType);
        }
        return *i->second;
    }

    void ResourceGroupManager::AddCreatedResource(ResourcePtr & p_resource, ResourceGroup & p_group)
    {
        uint32_t order = p_resource->GetCreator()->GetLoadingOrder();
        auto it = p_group.loadResourceOrderMap.find(order);

        if (it == p_group.loadResourceOrderMap.end())
        {
           it = p_group.loadResourceOrderMap.emplace(order, LoadUnloadResourceSet{}).first;
        }
        it->second.insert(p_resource);
    }

    void ResourceGroupManager::_NotifyResourceCreated(ResourcePtr& p_resource)
    {
        if (m_currentGroup && p_resource->GetGroup() == m_currentGroup->name)
        {
            AddCreatedResource(p_resource, *m_currentGroup);
        }
        else
        {
            ResourceGroup* group = GetResourceGroup(p_resource->GetGroup());
            if (group)
            {
                AddCreatedResource(p_resource, *group);
            }
        }
    }

    void ResourceGroupManager::_NotifyResourceRemoved(ResourcePtr& p_resource)
    {
        if (m_currentGroup)
        {
            // Do nothing - we're batch unloading so list will be cleared
        }
        else
        {
            // Find group
            ResourceGroup* group = GetResourceGroup(p_resource->GetGroup());
            if (group)
            {
                auto loadOrderIt = group->loadResourceOrderMap.find(p_resource->GetCreator()->GetLoadingOrder());
                if (loadOrderIt != group->loadResourceOrderMap.end())
                {
                    LoadUnloadResourceSet& resList = loadOrderIt->second;
                    auto listIt = resList.find(p_resource);
                    if (listIt != resList.end())
                        resList.erase(listIt);
                }
            }
        }
    }

    void ResourceGroupManager::_NotifyAllResourcesRemoved(AResourceManager* p_manager)
    {
        for (ResourceGroupMap::iterator resourceGroupIt = m_resourceGroupMap.begin();
            resourceGroupIt != m_resourceGroupMap.end(); ++resourceGroupIt)
        {
            // Iterate over all priorities
            for (ResourceGroup::LoadResourceOrderMap::iterator loadOrderIt = resourceGroupIt->second->loadResourceOrderMap.begin();
                loadOrderIt != resourceGroupIt->second->loadResourceOrderMap.end(); ++loadOrderIt)
            {
                // Iterate over all resources
                for (LoadUnloadResourceSet::iterator resourceIt = loadOrderIt->second.begin(); resourceIt != loadOrderIt->second.end();)
                {
                    if ((*resourceIt)->GetCreator() == p_manager)
                    {
                        // Increment first since iterator will be invalidated
                        LoadUnloadResourceSet::iterator deleted = resourceIt++;
                        loadOrderIt->second.erase(deleted);
                    }
                    else
                    {
                        ++resourceIt;
                    }
                }
            }
        }
    }
}
