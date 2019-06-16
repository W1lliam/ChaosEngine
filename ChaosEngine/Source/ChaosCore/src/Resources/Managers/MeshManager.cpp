#include "ChaosCore/Resources/Managers/MeshManager.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"

namespace Chaos
{
    MeshManager::MeshManager(const StringID& p_name) : AResourceManager(p_name, RESOURCE_TYPE, 300)
    {
    }

    MeshManager::~MeshManager()
    {
        RemoveAll();
    }

    AResource* MeshManager::CreateImpl(const std::string& p_name, const StringID& p_group)
    {
        return new (m_meshAllocator.Allocate(sizeof(Resources::Mesh), alignof(Resources::Mesh))) Resources::Mesh(this, p_name, p_group);
    }

    void MeshManager::RemoveImpl(AResource* p_resource)
    {
        static_cast<Resources::Mesh*>(p_resource)->~Mesh();
        m_meshAllocator.Free(p_resource);
    }

    void MeshManager::RemoveAllImpl()
    {
        m_meshAllocator.Clear();
    }

    Resources::MeshPtr MeshManager::GetMesh(const StringID& p_name, const StringID& p_group)
    {
        return std::static_pointer_cast<Resources::Mesh>(GetResource(p_name,p_group));
    }

	Resources::MeshPtr MeshManager::CreateQuad(const std::string& p_name, const StringID& p_group)
	{
		Resources::Mesh* quad = static_cast<Chaos::Resources::Mesh*>(CreateImpl(p_name, p_group));
		quad->MakeAsQuad();
		Resources::MeshPtr mesh(quad, std::bind(&MeshManager::RemoveImpl, this, std::placeholders::_1));
		AddImpl(std::reinterpret_pointer_cast<AResource>(mesh));
		return mesh;
	}
}