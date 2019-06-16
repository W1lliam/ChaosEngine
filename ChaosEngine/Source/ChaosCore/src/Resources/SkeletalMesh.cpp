#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Resources/SkeletalMesh.h"
#include "ChaosCore/Resources/Managers/AResourceManager.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"
#include "ChaosCore/Resources/AssetLoader.h"
namespace Chaos::Resources
{
	SkeletalMesh::~SkeletalMesh()
	{
		if (m_stream.is_open())
			m_stream.close();	

		m_animMesh.clear();
		m_animation.clear();
	}

	void SkeletalMesh::AddAnimation(std::shared_ptr<SkeletalMesh> p_otherSkelMesh, AnimationPtr p_otherAnimation)
	{
		if (verifySkeletonHierarchy(p_otherSkelMesh))
		{
			for (size_t i = 0; i < m_animation.size(); ++i)
			{
				if (m_animation[i]->GetName() == p_otherAnimation->GetName())
					return;
			}
			m_animation.push_back(p_otherAnimation);
		}
	}

	void SkeletalMesh::PreLoadImpl()
	{
	
	}
	void SkeletalMesh::PostLoadImpl()
	{
		for (AnimMeshPtr& meshe : m_animMesh)
		{
			meshe->PostLoad();
		}
		m_state.store(AResource::E_ResourceState::LOADED);
	}
	void SkeletalMesh::PreUnloadImpl()
	{

	}
	void SkeletalMesh::PostUnloadImpl()
	{

	}
	void SkeletalMesh::PrepareImpl()
	{
		m_stream = ResourceGroupManager::Get().OpenResource(m_name, m_group);
		m_skeleton = std::make_shared<Skeleton>();
		if (!AssetLoader::Validate(m_stream, AssetType::E_ASSETTYPE_SKELETALMESH))
		{
			throw std::exception((std::string("Asset " + m_name) + " is not valid").c_str());
			m_state.store(AResource::E_ResourceState::UNPREPARED);
		}
		m_state.store(AResource::E_ResourceState::PREPARED);
	}

	void SkeletalMesh::UnprepareImpl()
	{
		if (m_stream.is_open())
			m_stream.close();
	}
	void SkeletalMesh::LoadImpl()
	{
		m_state.store(AResource::E_ResourceState::LOADING);
		CHAOS_CORE_ASSERT(m_stream.is_open(), "Asset file has not been opened");
		std::string name;
		getline(m_stream, name, '\0');

		uint32_t BoneCount = 0;
		m_stream.read(reinterpret_cast<char*>(&BoneCount), sizeof(BoneCount));
		m_skeleton->m_bones.reserve(BoneCount);
		uint32_t id = 0;
		for (uint32_t bone = 0; bone < BoneCount; ++bone)
		{
			m_skeleton->m_bones.emplace_back(std::make_shared<Bone>());
			m_skeleton->m_bones.back()->Prepare(m_stream);
			m_skeleton->m_bones.back()->Load(m_stream, id);
			++id;
		}

		uint32_t meshCount = 0;
		m_stream.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));
		m_animMesh.reserve(meshCount);
		for (uint32_t mesh = 0; mesh < meshCount; ++mesh)
		{
			m_animMesh.emplace_back(std::make_shared<AnimMesh>());
			m_animMesh.back()->Prepare(m_stream);
			m_animMesh.back()->Load(m_stream, m_skeleton);
		}
		
		uint32_t animationCount = 0;
		m_stream.read(reinterpret_cast<char*>(&animationCount), sizeof(animationCount));
		m_animation.reserve(animationCount);
		for (uint32_t anim = 0; anim < animationCount; ++anim)
		{
			m_animation.emplace_back(std::make_shared<Animation>());
			m_animation.back()->Prepare(m_stream);
			m_animation.back()->Load(m_stream, m_skeleton);
		}
	}
	void SkeletalMesh::UnloadImpl()
	{
		for (BonePtr bone : m_skeleton->m_bones)
		{
			bone->Unload();
		}
		for (AnimationPtr animation : m_animation)
		{
			animation->Unload();
		}
		for (AnimMeshPtr animMesh : m_animMesh)
		{
			animMesh->Unload();
		}
		m_animation.clear();
		m_animMesh.clear();
	}

	bool SkeletalMesh::verifySkeletonHierarchy(std::shared_ptr<SkeletalMesh> p_otherSkelMesh)
	{
		SkeletalPtr p_other = p_otherSkelMesh->GetSkeleton();
		for (size_t i = 0; i < m_skeleton->GetBones().size(); ++i)
		{
			if (m_skeleton->GetBones()[i]->GetName() != p_other->GetBones()[i]->GetName())
				return false;
		}
		return true;
	}
}