#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "AssetFileConstants.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Resources/Material.h"


namespace Chaos
{
    class MaterialManager : public Singleton<MaterialManager>, public AResourceManager
    {
    public:
		MaterialManager(const StringID& p_name = "MaterialManager"_sid);
        ~MaterialManager();

        Resources::MaterialPtr GetMaterial(const StringID& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    protected:
        virtual AResource* CreateImpl(const std::string& p_name, const StringID & p_group)    override;
        virtual void       RemoveImpl(AResource*  p_resource)                               override;
        virtual void       RemoveAllImpl()                                                  override;

    protected:
        static constexpr uint32_t	MAX_MATERIAL_COUNT	{ 256 };
        static constexpr AssetType  RESOURCE_TYPE       { AssetType::E_ASSETTYPE_MATERIAL };

		Memory::PoolAllocator m_materialAllocator{ sizeof(Resources::Material) * MAX_MATERIAL_COUNT, sizeof(Resources::Material) , alignof(Resources::Material) };

    };
}