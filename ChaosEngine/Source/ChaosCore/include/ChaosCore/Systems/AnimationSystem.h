#pragma once
#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Animation/Animator.h"
namespace Chaos
{
	enum class AnimationState : uint8_t
	{
		START, 
		PAUSE,
		CHANGING,
		STOP
	};

	class AnimationSystem : public System
	{
		CHAOS_SYSTEM(AnimationSystem);
	
	public:
		AnimationSystem();
		~AnimationSystem();

		virtual void Start() override;
		virtual void Stop() override;
		virtual void Update(double dt) override;
		virtual void FixedUpdate(double dt) override;
		void AddAnimator(Animation::AnimatorPtr p_animator);
		inline const std::vector<Animation::AnimatorPtr>& GetAnimators() { return m_animators; };
        static  System&  Load(ReadStream& p_stream);
	private:
		std::vector<Animation::AnimatorPtr> m_animators;
	};
}