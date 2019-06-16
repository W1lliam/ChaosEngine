#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "AssetFileConstants.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Resources/AShader.h"
#include "ChaosCore/Resources/Buffers/IUniformBuffer.h"

namespace Chaos
{
    class ShaderManager : public Singleton<ShaderManager>, public AResourceManager
    {
    public:
        ShaderManager(const StringID& p_name = "ShaderManager"_sid);
        ~ShaderManager();

        Resources::ShaderPtr GetShader(const StringID& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Chaos::Graphics::Buffers::IUniformBuffer* matrixBuffer { nullptr };
		Chaos::Graphics::Buffers::IUniformBuffer* lightBuffer { nullptr };


    protected:
        virtual AResource* CreateImpl(const std::string& p_name, const StringID& p_group)   override;
        virtual void       RemoveImpl(AResource*  p_resource)                               override;
        virtual void       RemoveAllImpl()                                                  override;

    protected:
        static constexpr AssetType  RESOURCE_TYPE{ AssetType::E_ASSETTYPE_SHADER };
    };
}