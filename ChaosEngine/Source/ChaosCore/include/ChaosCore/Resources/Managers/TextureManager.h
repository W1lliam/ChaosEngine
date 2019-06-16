#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "AssetFileConstants.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Resources/ATexture.h"


namespace Chaos
{
	

    class TextureManager : public Singleton<TextureManager>, public AResourceManager
    {
    public:
        TextureManager(const StringID& p_name = "TextureManager"_sid);
        ~TextureManager();

		Resources::TexturePtr CreateTexture(const std::string& p_name, const StringID & p_group, uint32_t p_width, uint32_t p_heght, E_TextureType p_type = E_TextureType::RGBA8);
        Resources::TexturePtr GetTexture(const StringID& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    protected:
        virtual AResource* CreateImpl(const std::string& p_name, const StringID& p_group)    override;
        virtual AResource* CreateImpl(const std::string& p_name, const StringID& p_group, uint32_t& p_width, uint32_t& p_heght, E_TextureType p_type);
        virtual void       RemoveImpl(AResource*  p_resource)                               override;
        virtual void       RemoveAllImpl()                                                  override;

    protected:
        static constexpr AssetType  RESOURCE_TYPE       { AssetType::E_ASSETTYPE_TEXTURE };

    };
}