#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Resources/Managers/TextureManager.h"

namespace Chaos
{
    TextureManager::TextureManager(const StringID& p_name) : AResourceManager(p_name, RESOURCE_TYPE, 400)
    {
    }

    TextureManager::~TextureManager()
    {
        RemoveAll();
    }

	Resources::TexturePtr TextureManager::CreateTexture(const std::string& p_name, const StringID & p_group, uint32_t p_width, uint32_t p_heght, E_TextureType p_type)
	{
		ResourcePtr resource(CreateImpl(p_name, p_group, p_width, p_heght, p_type), std::bind(&TextureManager::RemoveImpl, this, std::placeholders::_1));
		AddImpl(resource);
		ResourceGroupManager::Get()._NotifyResourceCreated(resource);
		return std::static_pointer_cast<Resources::ATexture>(resource);
	}

    AResource* TextureManager::CreateImpl(const std::string& p_name, const StringID& p_group)
    {
        return Resources::ATexture::Create(this, p_name, p_group);
    }

	AResource* TextureManager::CreateImpl(const std::string& p_name, const StringID& p_group, uint32_t& p_width, uint32_t& p_heght, E_TextureType p_type)
	{
		return Resources::ATexture::Create(this, p_name, p_group, p_width, p_heght, p_type);
	}

    void TextureManager::RemoveImpl(AResource* p_resource)
    {
        static_cast<Resources::ATexture*>(p_resource)->~ATexture();
        Resources::ATexture::s_allocator.Free(p_resource);
    }

    void TextureManager::RemoveAllImpl()
    {
        Resources::ATexture::s_allocator.Clear();
    }

    Resources::TexturePtr TextureManager::GetTexture(const StringID& p_name, const StringID& p_group)
    {
        return std::static_pointer_cast<Resources::ATexture>(GetResource(p_name, p_group));
    }
}