#pragma once
#include "ChaosCore/Resources/Animation.h"
#include "ChaosCore/Animation/Pose.h"
#include "ChaosCore/Resources/Skeleton.h"
namespace Chaos
{
	namespace Animation
	{
		class AnimationBlender
		{
		public:
			AnimationBlender() = default;
			~AnimationBlender() = default;
			Pose CalculateBlendedPose(Chaos::Resources::SkeletalPtr p_skeleton);
			bool FinishedBlending();
			
			inline void SetCurrentPose(Pose& p_pose) { m_currentPose = p_pose; };
			inline void SetNextAnimation(Chaos::Resources::AnimationPtr p_next) { m_targetAnim = p_next; };	
		private:
			Chaos::Resources::AnimationKeyFrame GetTargetKeyFrame(Chaos::Resources::AnimationPtr p_changingAnimation);
			Pose InterPolateBlendingPoses(Chaos::Resources::AnimationKeyFrame& p_next, Chaos::Resources::SkeletalPtr p_currentSkeleton, const float& p_animationProgress);
			void IncreaseProgress();

		private:
			Chaos::Resources::AnimationPtr m_targetAnim;
			Pose m_currentPose;
			float m_progress = 0;
		};
	}
}