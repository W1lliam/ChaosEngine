#pragma once
#include "ChaosCore/Utils/Singleton.h"
#include "AssetFileConstants.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"
#include "ChaosCore/Resources/SkeletalMesh.h"

namespace Chaos
{
	class AnimationManager : public Singleton<AnimationManager>, public AResourceManager
	{
	public:
		AnimationManager(const StringID& p_name = "AnimationManager"_sid);
		~AnimationManager();

		Resources::SkeletalMeshPtr GetSkeletalMesh(const StringID& p_name, const StringID& p_group);

	protected:
		virtual AResource* CreateImpl(const std::string& p_name, const StringID& p_group)    override;
		virtual void       RemoveImpl(AResource*  p_resource)                                override;
		virtual void       RemoveAllImpl()                                                   override;

	protected:
		static constexpr AssetType  RESOURCE_TYPE{ AssetType::E_ASSETTYPE_SKELETALMESH };
		static constexpr uint8_t   MAX_SKELETAL_MESH_COUNT{ 10 };

		Memory::PoolAllocator m_SkeletalMeshAllocator{ sizeof(Resources::SkeletalMesh) * MAX_SKELETAL_MESH_COUNT, sizeof(Resources::SkeletalMesh) , alignof(Resources::SkeletalMesh) };
	};
}