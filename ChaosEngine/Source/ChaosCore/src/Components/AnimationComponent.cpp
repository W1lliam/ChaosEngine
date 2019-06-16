#include "ChaosCore/Components/AnimationComponent.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Systems/AnimationSystem.h"
#include "ChaosCore/Resources/Managers/AnimationManager.h"
#include "ChaosCore/GameObjects/EmptyGameObject.h" 
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Components/MaterialComponent.h"
#include "ChaosCore/GameObjects/SkeletalAnimationGameObject.h"

namespace Chaos
{
	CHAOS_DEFINE_COMPONENT_TYPEID(AnimationComponent);

	AnimationComponent::AnimationComponent(COMPONENT_INIT_PARAMS, Resources::SkeletalMeshPtr p_skelMesh) :
		COMPONENT_INIT(AnimationComponent), m_skeletalMesh(p_skelMesh), m_animator(std::make_shared<Animation::Animator>())
	{
		if (p_skelMesh)
		{
			m_animator->SetSkeletalMesh(m_skeletalMesh);
			SystemManager::Get().GetSystem<AnimationSystem>()->AddAnimator(m_animator);
		}
	}
	void AnimationComponent::SetSkeletalMesh(Resources::SkeletalMeshPtr p_skelMesh)
	{
		if (p_skelMesh)
		{
			m_skeletalMesh = p_skelMesh;
			m_animator->SetSkeletalMesh(m_skeletalMesh);
			SystemManager::Get().GetSystem<AnimationSystem>()->AddAnimator(m_animator);
			m_animator->ResetTextureSetting();
		}
	}
	const Resources::SkeletalMeshPtr AnimationComponent::GetSkeletalMesh() const
	{
		return m_skeletalMesh;
	}
	const Animation::AnimatorPtr AnimationComponent::GetAnimator() const
	{
		return m_animator;
	}
	bool AnimationComponent::Save(WriteStream & p_stream)
	{
		SerializeStringID(p_stream, const_cast<StringID&>(m_skeletalMesh->GetGroup()));
		SerializeStringID(p_stream, const_cast<StringID&>(m_skeletalMesh->GetName()));
		return true;
	}
	IComponent & AnimationComponent::Load(ReadStream & p_stream, const EntityID & p_entityID)
	{
		StringID    SkeletalMeshGroup;
		StringID    SkeletalMeshName;
		SerializeStringID(p_stream, SkeletalMeshGroup);
		SerializeStringID(p_stream, SkeletalMeshName);

		Resources::SkeletalMeshPtr skeletalMesh = AnimationManager::Get().GetSkeletalMesh(SkeletalMeshName, SkeletalMeshGroup);
		if (skeletalMesh)
		{
			return ComponentManager::Get().AddComponent<AnimationComponent>(p_entityID, skeletalMesh);
		}
		CHAOS_CORE_WARN("skeletalMesh {0} not found in group {1}", SkeletalMeshName, SkeletalMeshGroup);
		return ComponentManager::Get().AddComponent<AnimationComponent>(p_entityID, nullptr);
	}
	void AnimationComponent::PlayAnimation() const
	{
		if(m_skeletalMesh)
			m_animator->PlayAnimation();
	}
	void AnimationComponent::PauseAnimation() const
	{
		if (m_skeletalMesh)
			m_animator->PauseAnimation();
	}
	void AnimationComponent::StopAnimation() const
	{
		if (m_skeletalMesh)
			m_animator->StopAnimation();
	}

	void AnimationComponent::SetSkinningMode(Animation::SkinningMode p_mode)
	{
		m_animator->SetSkinningMode(p_mode);
	}
}