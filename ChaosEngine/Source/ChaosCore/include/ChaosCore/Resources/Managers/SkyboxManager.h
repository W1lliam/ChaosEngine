#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "AssetFileConstants.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Resources/ASkybox.h"


namespace Chaos
{
    class SkyboxManager : public Singleton<SkyboxManager>, public AResourceManager
    {
    public:
        SkyboxManager(const StringID& p_name = "SkyboxManager"_sid);
        ~SkyboxManager();

		Resources::SkyboxPtr CreateCubeMap(const std::string& p_name, const StringID & p_group, uint32_t p_width, uint32_t p_heght);
        Resources::SkyboxPtr GetCubeMap(const StringID& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    protected:
        virtual AResource* CreateImpl(const std::string& p_name, const StringID& p_group)    override;
        virtual AResource* CreateImpl(const std::string& p_name, const StringID& p_group, uint32_t& p_width, uint32_t& p_heght);
        virtual void       RemoveImpl(AResource*  p_resource)                               override;
        virtual void       RemoveAllImpl()                                                  override;

    protected:
        static constexpr AssetType  RESOURCE_TYPE       { AssetType::E_ASSETTYPE_SKYBOX };

    };
}