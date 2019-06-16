#include "ChaosCore\Resources\Managers\AnimationManager.h"
using namespace Chaos;

Chaos::AnimationManager::AnimationManager(const StringID & p_name) : AResourceManager(p_name, RESOURCE_TYPE, 500)
{
}

Chaos::AnimationManager::~AnimationManager()
{
	RemoveAll();
}

Resources::SkeletalMeshPtr Chaos::AnimationManager::GetSkeletalMesh(const StringID & p_name, const StringID & p_group)
{
	return std::static_pointer_cast<Resources::SkeletalMesh>(GetResource(p_name, p_group));
}

AResource * Chaos::AnimationManager::CreateImpl(const std::string & p_name, const StringID & p_group)
{
	return new (m_SkeletalMeshAllocator.Allocate(sizeof(Resources::SkeletalMesh), alignof(Resources::SkeletalMesh))) Resources::SkeletalMesh(this, p_name, p_group);
}

void Chaos::AnimationManager::RemoveImpl(AResource * p_resource)
{
	static_cast<Resources::SkeletalMesh*>(p_resource)->~SkeletalMesh();
	m_SkeletalMeshAllocator.Free(p_resource);
}

void Chaos::AnimationManager::RemoveAllImpl()
{
	m_SkeletalMeshAllocator.Clear();
}