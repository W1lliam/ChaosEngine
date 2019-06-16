#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "AssetFileConstants.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"
#include "ChaosCore/Resources/Mesh.h"

namespace Chaos
{
    class MeshManager : public Singleton<MeshManager>, public AResourceManager
    {
    public:
        MeshManager(const StringID& p_name = "MeshManager"_sid);
        ~MeshManager();

        Resources::MeshPtr GetMesh(const StringID& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Resources::MeshPtr CreateQuad(const std::string& p_name, const StringID& p_group = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    protected:
        virtual AResource* CreateImpl       (const std::string& p_name, const StringID& p_group)    override;
        virtual void       RemoveImpl       (AResource*  p_resource)                                override;
        virtual void       RemoveAllImpl    ()                                                      override;

    protected:
        static constexpr AssetType  RESOURCE_TYPE   {AssetType::E_ASSETTYPE_MESH};
        static constexpr uint32_t   MAX_MESH_COUNT  {256};

        Memory::PoolAllocator m_meshAllocator       {sizeof(Resources::Mesh) * MAX_MESH_COUNT, sizeof(Resources::Mesh) , alignof(Resources::Mesh)};
    };
}