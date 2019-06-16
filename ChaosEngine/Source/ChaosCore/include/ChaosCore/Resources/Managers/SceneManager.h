#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "AssetFileConstants.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Scene/Scene.h"


namespace Chaos
{
    class SceneManager : public Singleton<SceneManager>, public AResourceManager
    {
    public:
        SceneManager(const StringID& p_name = "SceneManager"_sid);
        ~SceneManager();

        ScenePtr GetScene(const StringID& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    protected:
        virtual AResource* CreateImpl(const std::string& p_name, const StringID & p_group)  override;
        virtual void       RemoveImpl(AResource*  p_resource)                               override;
        virtual void       RemoveAllImpl()                                                  override;

    protected:
        static constexpr AssetType  RESOURCE_TYPE{ AssetType::E_ASSETTYPE_SCENE };

    };
}