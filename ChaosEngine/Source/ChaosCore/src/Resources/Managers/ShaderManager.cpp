#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Resources/Managers/ShaderManager.h"

namespace Chaos
{
    ShaderManager::ShaderManager(const StringID& p_name) : AResourceManager(p_name, RESOURCE_TYPE, 500)
    {
    }

    ShaderManager::~ShaderManager()
    {
        RemoveAll();
    }

    AResource* ShaderManager::CreateImpl(const std::string& p_name, const StringID& p_group)
    {
        return Resources::AShader::Create(this, p_name, p_group);
    }

    void ShaderManager::RemoveImpl(AResource* p_resource)
    {
        static_cast<Resources::AShader*>(p_resource)->~AShader();
        Resources::AShader::s_allocator.Free(p_resource);
    }

    void ShaderManager::RemoveAllImpl()
    {
        Resources::AShader::s_allocator.Clear();
    }

    Resources::ShaderPtr ShaderManager::GetShader(const StringID& p_name, const StringID& p_group)
    {
        return std::static_pointer_cast<Resources::AShader>(GetResource(p_name, p_group));
    }
}