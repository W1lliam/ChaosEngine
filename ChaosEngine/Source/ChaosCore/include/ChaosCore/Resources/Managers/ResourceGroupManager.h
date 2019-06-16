#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "ChaosCore/Utils/StringID.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Resources/AResource.h"
#include "AssetFileConstants.h"

namespace Chaos
{
    class AResourceManager;

    class ResourceGroupManager : public Singleton<ResourceGroupManager>
    {

    public:
        static const     StringID DEFAULT_RESOURCE_GROUP_NAME;
        static constexpr uint32_t RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS  {2};
        struct ResourceDeclaration
        {
            std::string         resourceName;
            AssetType           resourceType;
            ResourceDeclaration(const std::string& p_resourceName, const AssetType& p_resourceType) : resourceName { p_resourceName }, resourceType{ p_resourceType } {}
        };
        using ResourceDeclarationList   = std::list<ResourceDeclaration>;
        using ResourcePtr               = std::shared_ptr<AResource>;
    
    public:
        ResourceGroupManager();
        ~ResourceGroupManager();

        void CreateResourceGroup            (const StringID& p_group, const bool p_inGlobalPool = true);
        void DestroyResourceGroup           (const StringID& p_group);

        void AddResourceLocation            (const std::string& p_location, const StringID& p_group = DEFAULT_RESOURCE_GROUP_NAME, bool p_recursive = false, bool p_readOnly = true);
        void RemoveResourceLocation         (const std::string& p_location, const StringID& p_group = DEFAULT_RESOURCE_GROUP_NAME);

        void DeclareResource                (const std::string& p_name, AssetType p_resourceType, const StringID& p_group);
        void LoadResourceDeclaration        (const std::string& p_path);

        void UndeclareResource              (const std::string& p_name, const StringID& p_group);
        
        void InitializeResourceGroup        (const StringID& p_group);
        void InitializeAllResourceGroups    ();

        void PrepareResourceGroup           (const StringID& p_group, bool p_prepareMainResources = true);

        void LoadAllResourceGroups          ();
        void LoadResourceGroup              (const StringID& p_group, bool p_loadMainResources = true);
        void UnloadResourceGroup            (const StringID& p_group, bool p_reloadableOnly = true);

        void ClearResourceGroup             (const StringID& p_group);

        void RegisterResourceManager        (AssetType p_resourceType, AResourceManager* p_resourceManager);
        void UnregisterResourceManager      (AssetType p_resourceType);

        std::ifstream OpenResource          (const StringID& p_resource, const StringID& p_group = DEFAULT_RESOURCE_GROUP_NAME, int p_mode = std::ios::binary | std::ios::in);
		std::string	  GetResourcePath		(const StringID& p_resource, const StringID& p_group = DEFAULT_RESOURCE_GROUP_NAME);
		bool IsResourceGroupInGlobalPool    (const StringID& p_group) const;


        void _NotifyResourceCreated(ResourcePtr& p_resource);
        void _NotifyResourceRemoved(ResourcePtr& p_resource);
        void _NotifyAllResourcesRemoved(AResourceManager* p_manager);



    protected:
        static constexpr uint8_t MAX_RESOURCE_GROUP     {32};
        struct ResourceLocation
        {
            filesystem::path folder;
            bool recursive;
            bool readOnly;
            ResourceLocation(filesystem::path p_folder, bool p_recursive, bool p_readOnly) : folder{p_folder}, recursive{p_recursive}, readOnly{p_readOnly} {}
        };

        using LocationList          = std::list<ResourceLocation>;
        using ResourceLocationIndex = std::map<StringID, filesystem::path>;
        using LoadUnloadResourceSet = std::unordered_set<ResourcePtr>;

        struct ResourceGroup
        {
            using LoadResourceOrderMap = std::map<uint32_t, LoadUnloadResourceSet>;

            enum class E_Status : uint8_t
            {
                UNINITIALIZED,
                INITIALIZING,
                INITIALIZED,
                LOADING,
                LOADED
            };

            StringID                name;
            E_Status                status;
            bool                    inGlobalPool;

            LocationList            locationList;
            ResourceLocationIndex   resourceIndex;
            ResourceDeclarationList resourceDeclarations;
            LoadResourceOrderMap    loadResourceOrderMap;
            StringID                worldGeometry;
        };
        
        using ResourceGroupMap      = std::map<StringID, ResourceGroup*>;
        using ResourceManagerMap    = std::map<AssetType, AResourceManager*>;

    protected:
        ResourceGroup*      GetResourceGroup        (const StringID& p_group) const;
        void                CreateDeclaredResources (ResourceGroup* p_group);
        void                DropGroupContents       (ResourceGroup* p_group);
        void                DeleteGroup             (ResourceGroup* p_group);
        AResourceManager&   GetResourceManager      (AssetType p_resourceType);

        void                AddCreatedResource      (ResourcePtr& p_resource, ResourceGroup& p_group);
    
    protected:
        Memory::PoolAllocator   m_resourceGroupAllocator    {sizeof(ResourceGroup) * MAX_RESOURCE_GROUP,  sizeof(ResourceGroup), alignof(ResourceGroup) };
        ResourceManagerMap      m_resourceManagerMap;
        ResourceGroupMap        m_resourceGroupMap;
        StringID                m_worldGroupName            {DEFAULT_RESOURCE_GROUP_NAME};
        ResourceGroup*          m_currentGroup              {nullptr};
    };
}