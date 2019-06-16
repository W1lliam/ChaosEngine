#pragma once
#include "ChaosCore/Utils/StringID.h"
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"

namespace Chaos
{
    class AResourceManager
    {
    public:
        using ResourceMap        = std::unordered_map<StringID, ResourcePtr>;
        using GroupedResourceMap = std::unordered_map<StringID, ResourceMap>;

                ResourcePtr                     CreateResource      (const std::string& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
                std::pair<ResourcePtr, bool>    CreateOrRetrieve    (const std::string& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        virtual ResourcePtr                     Prepare             (const std::string& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        virtual ResourcePtr                     Load                (const std::string& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        virtual void                            ReloadAll           ();
        virtual void                            ReloadUnreferenced  ();

                void                            Unload              (const StringID& p_name);
                void                            Unload              (ResourcePtr& p_resource);
        virtual void                            UnloadAll           ();
        virtual void                            UnloadUnreferenced  ();

                void                            Remove              (const StringID& p_name);
                void                            Remove              (ResourcePtr& p_resource);
        virtual void                            RemoveAll           ();
        virtual void                            RemoveUnreferenced  ();

                ResourcePtr                     GetResource         (const StringID& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        virtual const StringID&                 GetName             () const { return m_name;           }
        virtual uint32_t                        GetLoadingOrder     () const { return m_loadOrder;      }
        virtual AssetType                       GetResourceType     () const { return m_resourceType;   }
        inline  const ResourceMap&              GetAllResources     () const { return m_resources;      };


    protected:
                AResourceManager(const StringID& p_name, AssetType p_resourceType, uint32_t p_loadOrder);
                virtual ~AResourceManager();

        virtual AResource*  CreateImpl  (const std::string& p_name, const StringID& p_group) = 0;
        virtual void        RemoveImpl(AResource* p_resource) = 0;
        virtual void        RemoveAllImpl() = 0;

        virtual void        AddImpl     (ResourcePtr& p_resource);


        StringID            m_name;
        uint32_t            m_loadOrder;
        ResourceMap         m_resources;
        GroupedResourceMap  m_groupedResources;
        AssetType           m_resourceType;
    };
}