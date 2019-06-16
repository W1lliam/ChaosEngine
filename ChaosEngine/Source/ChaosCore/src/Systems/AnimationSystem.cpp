#include "ChaosCore\Systems\AnimationSystem.h"
#include "ChaosCore/Resources/Material.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include <cmath>

namespace Chaos
{
	CHAOS_DEFINE_SYSTEM_TYPEID(AnimationSystem);

	AnimationSystem::AnimationSystem()
	{
	}

	AnimationSystem::~AnimationSystem()
	{
	}

	void AnimationSystem::Start()
	{
	}

	void AnimationSystem::Stop()
	{
	}

	void AnimationSystem::Update(double dt)
	{
		for (Animation::AnimatorPtr& anim : m_animators)
			anim->UpdateAnimation(dt);
	}
	void AnimationSystem::FixedUpdate(double dt)
	{
	}
	void AnimationSystem::AddAnimator(Animation::AnimatorPtr p_animator)
	{
		auto& it = std::find_if(m_animators.begin(), m_animators.end(), [&p_animator] (const Animation::AnimatorPtr& animator)
		{
			return animator == p_animator;
		});

		if(it == m_animators.end())
			m_animators.push_back(p_animator);
	}
    System & AnimationSystem::Load(ReadStream & p_stream)
    {
        auto system = SystemManager::Get().AddSystem<AnimationSystem>();
        static_cast<System*>(system)->Load(p_stream);
        return *system;
    }
}