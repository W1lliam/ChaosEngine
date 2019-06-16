#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Resources/Managers/SkyboxManager.h"

namespace Chaos
{
    SkyboxManager::SkyboxManager(const StringID& p_name) : AResourceManager(p_name, RESOURCE_TYPE, 400)
    {
    }

    SkyboxManager::~SkyboxManager()
    {
        RemoveAll();
    }

	Resources::SkyboxPtr SkyboxManager::CreateCubeMap(const std::string& p_name, const StringID & p_group, uint32_t p_width, uint32_t p_heght)
	{
		ResourcePtr resource(CreateImpl(p_name, p_group, p_width, p_heght), std::bind(&SkyboxManager::RemoveImpl, this, std::placeholders::_1));
		AddImpl(resource);
		ResourceGroupManager::Get()._NotifyResourceCreated(resource);
		return std::static_pointer_cast<Resources::ASkybox>(resource);
	}

    AResource* SkyboxManager::CreateImpl(const std::string& p_name, const StringID& p_group)
    {
        return Resources::ASkybox::Create(this, p_name, p_group);
    }

	AResource* SkyboxManager::CreateImpl(const std::string& p_name, const StringID& p_group, uint32_t& p_width, uint32_t& p_heght)
	{
		return Resources::ASkybox::Create(this, p_name, p_group, p_width, p_heght);
	}

    void SkyboxManager::RemoveImpl(AResource* p_resource)
    {
        static_cast<Resources::ASkybox*>(p_resource)->~ASkybox();
        Resources::ASkybox::s_allocator.Free(p_resource);
    }

    void SkyboxManager::RemoveAllImpl()
    {
        Resources::ASkybox::s_allocator.Clear();
    }

    Resources::SkyboxPtr SkyboxManager::GetCubeMap(const StringID& p_name, const StringID& p_group)
    {
        return std::static_pointer_cast<Resources::ASkybox>(GetResource(p_name, p_group));
    }
}