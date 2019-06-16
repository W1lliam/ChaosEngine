#pragma once
#include "ChaosCore/Animation/PoseCalculator.h"
#include "ChaosCore/Resources/SkeletalMesh.h"
#include "ChaosCore/Animation/AnimationBlender.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Events/KeyEvents.h"
#include "ChaosCore/Resources/Material.h"
namespace Chaos
{
	namespace Animation
	{
		enum class SkinningMode :uint8_t
		{
			LINEAR,
			DUALQUAT,
		};

		enum class AnimationState : uint8_t
		{
			START,
			PAUSE,
			CHANGING,
			STOP
		};
		struct FinalPoseData
		{
			std::vector<Maths::Vector3> tposPostion;
			std::vector<Maths::Vector3> animPosition;
			std::vector<Maths::Vector4> finalRotaion;
			void ResetFinalPoseData()
			{
				tposPostion.clear();
				animPosition.clear();
				finalRotaion.clear();
			}
			void ResizeDataContainer(uint32_t& p_size)
			{
				tposPostion.reserve(p_size);
				animPosition.reserve(p_size);
				finalRotaion.reserve(p_size);
			}
		};
		
		class Animator : public Events::EventListener
		{
		public:
			Animator();
			Animator(SkinningMode p_mode);
			~Animator() = default;

			void IncreaseCurrentAnimTime(double dt);

			void PlayAnimation();
			void PauseAnimation();
			void StopAnimation();
			void UpdateAnimation(double dt);
			void ResetTextureSetting();
			void SetSkinningMode(SkinningMode p_mode);
			inline void SetAnimationstate(AnimationState p_state) { m_state = p_state; };
			void SetSkeletalMesh(Resources::SkeletalMeshPtr& p_skeletalMesh);
			std::string GetSkinningMode();

		private:
			void SetCurrentAnimation(Resources::AnimationPtr p_animation);
			bool OnKeyPressed(const Events::KeyPressedEvent& p_event);
			bool OnKeyReleased(const Events::KeyReleasedEvent& p_event);
			void ChangeAnimation(E_ChaosKeyCode& p_keycode);
			void SendDataToShader(std::vector<Maths::Vector3>& p_tposPostion,
								  std::vector<Maths::Vector3>& p_animPosition,
								  std::vector<Maths::Vector4>& p_finalRotaion);
			void SendDQDataToShader(std::vector<Maths::Vector3>& p_finalPosition,
									std::vector<Maths::Vector4>& p_finalRotaion);
		private:
			Animation::PoseCalculator m_poseCalculator;
			Animation::AnimationBlender m_animBlender;
			Resources::SkeletalMeshPtr m_skeletalMesh;
			Resources::AnimationPtr m_currentAnimation;
			Resources::AnimationPtr m_nextAnimation;

			FinalPoseData m_finalPoseData;
			SkinningMode m_skinning = SkinningMode::DUALQUAT;
			AnimationState m_state = AnimationState::STOP;
			float m_currentAnimationTime = 0;
			bool m_firstTimeChanging = true;
		};
		using AnimatorPtr = std::shared_ptr<Animator>;
	}
}