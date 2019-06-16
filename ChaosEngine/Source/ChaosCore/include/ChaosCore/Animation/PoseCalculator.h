#pragma once
#include <ChaosMaths.h>
#include <memory>
#include "ChaosCore/Animation/Pose.h"
#include "ChaosCore/Animation/DualQuaternionPose.h"
#include "ChaosCore/Resources/Animation.h"
namespace Chaos
{
	namespace Animation
	{
		class PoseCalculator
		{
		public: 
			PoseCalculator() = default;
			~PoseCalculator() = default;
			void AnimationChanged();
			Pose CalculateFinalPose(Chaos::Resources::AnimationPtr p_currentAnim, Chaos::Resources::SkeletalPtr p_currentSkeleton, const float& p_animTime);
		private:
			Chaos::Resources::AnimationKeyFrame  m_Prev;
			Chaos::Resources::AnimationKeyFrame  m_Next;
			Pose InterpolateTwoPoses(Chaos::Resources::AnimationKeyFrame& p_outPrev, Chaos::Resources::AnimationKeyFrame& p_outNext, Chaos::Resources::SkeletalPtr p_currentSkeleton, const float& p_animationProgress);
			
			float calculateProgression(const float& p_start, const float& p_end, const float& p_animTime);
			void GetPrevNextFrame(Chaos::Resources::AnimationKeyFrame& p_outPrev, Chaos::Resources::AnimationKeyFrame& p_outNext, Chaos::Resources::AnimationPtr p_currentAnim, const float& p_animTime);
		private:
			bool m_firstTimeUpdate = true;
		};
	}
}