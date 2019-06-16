#include "ChaosCore\Animation\PoseCalculator.h"
#include "ChaosCore/Resources/SkeletalMesh.h"

namespace Chaos
{
	void Animation::PoseCalculator::AnimationChanged()
	{
		m_firstTimeUpdate = true;
	}

	Pose Animation::PoseCalculator::CalculateFinalPose(Chaos::Resources::AnimationPtr p_currentAnim, Chaos::Resources::SkeletalPtr p_currentSkeleton, const float & p_animTime)
	{
			GetPrevNextFrame(m_Prev, m_Next, p_currentAnim, p_animTime);
			float progressTime = calculateProgression(static_cast<float>(m_Prev.timeStamp), static_cast<float>(m_Next.timeStamp), p_animTime);
			return InterpolateTwoPoses(m_Prev, m_Next, p_currentSkeleton, progressTime);
	}
	
	Pose Animation::PoseCalculator::InterpolateTwoPoses(Chaos::Resources::AnimationKeyFrame & p_outPrev, Chaos::Resources::AnimationKeyFrame & p_outNext, Chaos::Resources::SkeletalPtr p_currentSkeleton, const float & p_animationProgress)
	{
		Pose ToReturn;
		ToReturn.SetSkeletonPtr(p_currentSkeleton);
		for (size_t i = 0; i < p_currentSkeleton->GetBones().size(); ++i)
		{
			std::string currentBoneName = p_currentSkeleton->GetBones()[i]->GetName();
			Transform transform(Maths::Vector3::Lerp(p_outPrev.PosAtTime[i].second, p_outNext.PosAtTime[i].second, p_animationProgress), Maths::Quaternion::NLerp(p_outPrev.RotAtTime[i].second, p_outNext.RotAtTime[i].second, p_animationProgress));
			ToReturn.AddTransform(currentBoneName, transform);
		}
		return ToReturn;
	}

	float Chaos::Animation::PoseCalculator::calculateProgression(const float& p_start, const float& p_end, const float & p_animTime)
	{
		return (p_animTime - p_start) / (p_end - p_start);
	}
	void Animation::PoseCalculator::GetPrevNextFrame(Chaos::Resources::AnimationKeyFrame & p_outPrev, Chaos::Resources::AnimationKeyFrame & p_outNext, Chaos::Resources::AnimationPtr p_currentAnim, const float & p_animTime)
	{
			p_outPrev = p_currentAnim->GetAnimNodePerFrame()[0];
			p_outNext = p_currentAnim->GetAnimNodePerFrame()[0];
		for (size_t i = 1; i < p_currentAnim->GetAnimNodePerFrame().size(); i++)
		{
			p_outNext = p_currentAnim->GetAnimNodePerFrame()[i];
			if (p_outNext.timeStamp > p_animTime)
				break;

			p_outPrev = p_currentAnim->GetAnimNodePerFrame()[i];
		}
	}
}