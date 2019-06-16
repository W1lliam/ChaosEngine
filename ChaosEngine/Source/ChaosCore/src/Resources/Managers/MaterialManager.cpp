#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Resources/Managers/MaterialManager.h"

namespace Chaos
{
	MaterialManager::MaterialManager(const StringID& p_name) : AResourceManager(p_name, RESOURCE_TYPE, 400)
    {
    }

	MaterialManager::~MaterialManager()
    {
        RemoveAll();
    }

    AResource* MaterialManager::CreateImpl(const std::string& p_name, const StringID& p_group)
    {
        return new (m_materialAllocator.Allocate(sizeof(Resources::Material), alignof(Resources::Material))) Resources::Material(this, p_name, p_group);
    }

    void MaterialManager::RemoveImpl(AResource* p_resource)
    {
        static_cast<Resources::Material*>(p_resource)->~Material();
		m_materialAllocator.Free(p_resource);
    }

    void MaterialManager::RemoveAllImpl()
    {
		m_materialAllocator.Clear();
    }

    Resources::MaterialPtr MaterialManager::GetMaterial(const StringID& p_name, const StringID& p_group)
    {
        return std::static_pointer_cast<Resources::Material>(GetResource(p_name, p_group));
    }
}