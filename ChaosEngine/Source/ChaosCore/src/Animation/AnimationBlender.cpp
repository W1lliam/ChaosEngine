#include "ChaosCore\Animation\AnimationBlender.h"

namespace Chaos
{

	Pose Animation::AnimationBlender::CalculateBlendedPose( Chaos::Resources::SkeletalPtr p_skeleton)
	{
		IncreaseProgress();
		Chaos::Resources::AnimationKeyFrame target = GetTargetKeyFrame(m_targetAnim);
		return InterPolateBlendingPoses(target, p_skeleton, m_progress);
	}

	bool Chaos::Animation::AnimationBlender::FinishedBlending()
	{
		bool flag = false;
		if (m_progress >= 1.0f)
		{
			m_progress = 0;
			flag = true;
		}
		return flag;
	}


	Chaos::Resources::AnimationKeyFrame Animation::AnimationBlender::GetTargetKeyFrame(Chaos::Resources::AnimationPtr p_changingAnimation)
	{
		return p_changingAnimation->GetAnimNodePerFrame()[0];
	}

	Pose Animation::AnimationBlender::InterPolateBlendingPoses(Chaos::Resources::AnimationKeyFrame & p_next, Chaos::Resources::SkeletalPtr p_currentSkeleton, const float & p_animationProgress)
	{
		Pose FinalPose;
		FinalPose.SetSkeletonPtr(p_currentSkeleton);
		for (size_t i = 0; i < p_currentSkeleton->GetBones().size(); ++i)
		{
			std::string currentBoneName = p_currentSkeleton->GetBones()[i]->GetName();
			Transform transform(Maths::Vector3::Lerp(m_currentPose.GetLocalTranslation(currentBoneName), p_next.PosAtTime[i].second, p_animationProgress), 
								Maths::Quaternion::NLerp(m_currentPose.GetLocalRotation(currentBoneName), p_next.RotAtTime[i].second, p_animationProgress));
			
			FinalPose.AddTransform(currentBoneName, transform);
		}
		return FinalPose;
	}

	void Chaos::Animation::AnimationBlender::IncreaseProgress()
	{
		m_progress += 0.2f;
	}
}