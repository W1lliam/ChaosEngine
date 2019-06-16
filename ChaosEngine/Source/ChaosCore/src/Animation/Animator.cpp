#include "ChaosCore\Animation\Animator.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Systems/AnimationSystem.h"
#include "ChaosCore/Resources/Managers/MaterialManager.h"
#include "ChaosCore/Resources/Managers/TextureManager.h"
#include "ChaosCore/Resources/Managers/ShaderManager.h"
#include "ChaosCore/Resources/Material.h"
#include "ChaosCore/ECS/Managers/ComponentManager.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Components/MaterialComponent.h"
#include "ChaosCore/Components/AnimationComponent.h"
namespace Chaos
{
	Chaos::Animation::Animator::Animator()
	{
	}

	Chaos::Animation::Animator::Animator(SkinningMode p_mode) : m_skinning(p_mode)
	{
	}

	void Chaos::Animation::Animator::IncreaseCurrentAnimTime(double dt)
	{
		m_currentAnimationTime += static_cast<float>(dt * m_currentAnimation->GetTicksPerSecond());
		if (m_currentAnimationTime > m_currentAnimation->GetDuration())
			m_currentAnimationTime = static_cast<float>(fmod(m_currentAnimationTime, m_currentAnimation->GetDuration()));
	}

	void Chaos::Animation::Animator::PlayAnimation()
	{
		m_state = AnimationState::START;
	}

	void Chaos::Animation::Animator::PauseAnimation()
	{
		m_state = AnimationState::PAUSE;
	}

	void Chaos::Animation::Animator::StopAnimation()
	{
			m_state = AnimationState::STOP;
			m_currentAnimationTime = 0;
	}

	void Chaos::Animation::Animator::UpdateAnimation(double dt)
	{
		if (m_state == AnimationState::STOP)
		{
			m_currentAnimationTime = 0.f;
		}
		else if(m_state == AnimationState::START)
			IncreaseCurrentAnimTime(dt);

		Pose currentPose = m_poseCalculator.CalculateFinalPose(m_currentAnimation, m_skeletalMesh->GetSkeleton(), m_currentAnimationTime);
		std::vector<Transform> AnimTransform = currentPose.GetAnimWorldTransform();
		std::vector<Transform> TPosTransform = m_skeletalMesh->GetSkeleton()->GetWorldTposTransform();
		if (m_state == AnimationState::CHANGING)
		{
			m_animBlender.SetCurrentPose(currentPose);
			m_animBlender.SetNextAnimation(m_nextAnimation);
			Pose blendPose = m_animBlender.CalculateBlendedPose(m_skeletalMesh->GetSkeleton());
			AnimTransform = blendPose.GetAnimWorldTransform();
		
			if (m_animBlender.FinishedBlending())
			{
				m_currentAnimationTime = 0;
				PlayAnimation();
			}
		}

		if (m_skinning == SkinningMode::LINEAR)
		{
				m_finalPoseData.ResetFinalPoseData();
				for (size_t i = 0; i < AnimTransform.size(); ++i)
				{
					m_finalPoseData.tposPostion.emplace_back(TPosTransform[i].GetPosition());
					m_finalPoseData.animPosition.emplace_back(AnimTransform[i].GetPosition());
					Maths::Quaternion finalQuat = (AnimTransform[i].GetRotationQuaternion() * Maths::Quaternion::InverseQuat(TPosTransform[i].GetRotationQuaternion())).GetNormalized();
					m_finalPoseData.finalRotaion.emplace_back(finalQuat.GetX(), finalQuat.GetY(), finalQuat.GetZ(), finalQuat.GetRealValue());
				}
				SendDataToShader(m_finalPoseData.tposPostion, m_finalPoseData.animPosition, m_finalPoseData.finalRotaion);
		}
		else if (m_skinning == SkinningMode::DUALQUAT)
		{
			std::vector<Maths::Matrix4> FinalTransform;
			for (size_t i = 0; i < AnimTransform.size(); ++i)
			{
				FinalTransform.push_back(AnimTransform[i].GetTransform() * TPosTransform[i].GetTransform().Inverse());
			}
			m_finalPoseData.ResetFinalPoseData();
			for (size_t i = 0; i < AnimTransform.size(); ++i)
			{
				m_finalPoseData.tposPostion.emplace_back(FinalTransform[i].m03, FinalTransform[i].m13, FinalTransform[i].m23);
				Maths::Quaternion finalQuat = Maths::Quaternion::FromMatrix4(FinalTransform[i]);
				m_finalPoseData.finalRotaion.emplace_back(finalQuat.GetX(), finalQuat.GetY(), finalQuat.GetZ(),
					finalQuat.GetRealValue());
			}
			SendDQDataToShader(m_finalPoseData.tposPostion, m_finalPoseData.finalRotaion);
		}
	}

	void Animation::Animator::ResetTextureSetting()
	{
		m_firstTimeChanging = true;
	}

	void Chaos::Animation::Animator::SetSkinningMode(SkinningMode p_mode)
	{
		ComponentManager&   componentManager = ComponentManager::Get();
		auto                scene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
		for (auto animIt = scene->begin<AnimationComponent>(); animIt != scene->end<AnimationComponent>(); animIt++)
		{
			AnimationComponent* SkelMeshComponent = reinterpret_cast<AnimationComponent*>(animIt->second);
			if (SkelMeshComponent && SkelMeshComponent->GetSkeletalMesh())
			{
				if (SkelMeshComponent && SkelMeshComponent->IsActive()
					&& EntityManager::Get().GetEntity(SkelMeshComponent->GetOwnerID())->IsActive()
					&& SkelMeshComponent->GetSkeletalMesh()->GetState() == AResource::E_ResourceState::LOADED && SkelMeshComponent->GetSkeletalMesh()->GetName() == m_skeletalMesh->GetName())
				{
					for (Resources::AnimMeshPtr& pMesh : SkelMeshComponent->GetSkeletalMesh()->GetAnimationMesh())
					{
						MaterialComponent*  materialComponent = componentManager.GetComponent<MaterialComponent>(pMesh->GetOwnerID());
						if (materialComponent && materialComponent->IsActive() && !m_firstTimeChanging)
						{
							Chaos::Resources::TexturePtr first = materialComponent->GetMaterial()->GetTexture("uTexture");
							Chaos::Resources::TexturePtr second = materialComponent->GetMaterial()->GetTexture("metallic");
							Chaos::Resources::TexturePtr third = materialComponent->GetMaterial()->GetTexture("roughness");
							Chaos::Resources::TexturePtr fourth = materialComponent->GetMaterial()->GetTexture("emissive");
							if (p_mode == SkinningMode::DUALQUAT)
							{
								materialComponent->GetMaterial()->SetShader(ShaderManager::Get().GetShader("DualQuaternionSkinningShader"_sid, "Editor"_sid));
							}
							else if (p_mode == SkinningMode::LINEAR)
							{
								materialComponent->GetMaterial()->SetShader(ShaderManager::Get().GetShader("LinearSkinningShader"_sid, "Editor"_sid));
							}
							materialComponent->GetMaterial()->SetTexture("uTexture", first);
							materialComponent->GetMaterial()->SetTexture("metallic", second);
							materialComponent->GetMaterial()->SetTexture("roughness", third);
							materialComponent->GetMaterial()->SetTexture("emissive", fourth);
						}
						else
						{
							if (p_mode == SkinningMode::DUALQUAT)
							{
								materialComponent->GetMaterial()->SetShader(ShaderManager::Get().GetShader("DualQuaternionSkinningShader"_sid, "Editor"_sid));
							}
							else if (p_mode == SkinningMode::LINEAR)
							{
								materialComponent->GetMaterial()->SetShader(ShaderManager::Get().GetShader("LinearSkinningShader"_sid, "Editor"_sid));
							}
							materialComponent->GetMaterial()->SetTexture("uTexture", TextureManager::Get().GetTexture("Revolver_AAO_s"_sid, "Editor"_sid));
							materialComponent->GetMaterial()->SetTexture("metallic", TextureManager::Get().GetTexture("Revolver_M"_sid, "Editor"_sid));
							materialComponent->GetMaterial()->SetTexture("roughness", TextureManager::Get().GetTexture("Revolver_R"_sid, "Editor"_sid));
							materialComponent->GetMaterial()->SetTexture("emissive", TextureManager::Get().GetTexture("Revolver_E"_sid, "Editor"_sid));
							m_firstTimeChanging = false;
						}
					}
				}
			}
		}
		m_skinning = p_mode;
	}
	void Chaos::Animation::Animator::SendDataToShader(std::vector<Maths::Vector3>& p_tposPostion, std::vector<Maths::Vector3>& p_animPosition, std::vector<Maths::Vector4>& p_finalRotaion)
	{
		ComponentManager&   componentManager = ComponentManager::Get();
		auto                scene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
		for (auto animIt = scene->begin<AnimationComponent>(); animIt != scene->end<AnimationComponent>(); animIt++)
		{
			AnimationComponent* SkelMeshComponent = reinterpret_cast<AnimationComponent*>(animIt->second);
			if (SkelMeshComponent && SkelMeshComponent->GetSkeletalMesh())
			{
				if (SkelMeshComponent && SkelMeshComponent->GetSkeletalMesh() && SkelMeshComponent->IsActive()
					&& EntityManager::Get().GetEntity(SkelMeshComponent->GetOwnerID())->IsActive()
					&& SkelMeshComponent->GetSkeletalMesh()->GetState() == AResource::E_ResourceState::LOADED && SkelMeshComponent->GetSkeletalMesh()->GetName() == m_skeletalMesh->GetName())
				{
					for (Resources::AnimMeshPtr& pMesh : SkelMeshComponent->GetSkeletalMesh()->GetAnimationMesh())
					{
						MaterialComponent*  materialComponent = componentManager.GetComponent<MaterialComponent>(pMesh->GetOwnerID());
						if (materialComponent && materialComponent->IsActive() && materialComponent->GetMaterial())
						{
							materialComponent->GetMaterial()->SetUniform("TPosPositions[0]", p_tposPostion);
							materialComponent->GetMaterial()->SetUniform("AnimPositions[0]", p_animPosition);
							materialComponent->GetMaterial()->SetUniform("FinalRotations[0]", p_finalRotaion);
						}
					}
				}
			}
		}
	}

	void Animation::Animator::SendDQDataToShader(std::vector<Maths::Vector3>& p_finalPosition, std::vector<Maths::Vector4>& p_finalRotaion)
	{
		ComponentManager&   componentManager = ComponentManager::Get();
		auto                scene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
		for (auto animIt = scene->begin<AnimationComponent>(); animIt != scene->end<AnimationComponent>(); animIt++)
		{
			AnimationComponent* SkelMeshComponent = reinterpret_cast<AnimationComponent*>(animIt->second);
			if (SkelMeshComponent && SkelMeshComponent->GetSkeletalMesh())
			{
				if (SkelMeshComponent->IsActive()
					&& EntityManager::Get().GetEntity(SkelMeshComponent->GetOwnerID())->IsActive()
					&& SkelMeshComponent->GetSkeletalMesh()->GetState() == AResource::E_ResourceState::LOADED && SkelMeshComponent->GetSkeletalMesh()->GetName() == m_skeletalMesh->GetName())
				{
					for (Resources::AnimMeshPtr& pMesh : SkelMeshComponent->GetSkeletalMesh()->GetAnimationMesh())
					{
						MaterialComponent*  materialComponent = componentManager.GetComponent<MaterialComponent>(pMesh->GetOwnerID());
						if (materialComponent && materialComponent->IsActive() && materialComponent->GetMaterial())
						{
							materialComponent->GetMaterial()->SetUniform("FinalPositions[0]", p_finalPosition);
							materialComponent->GetMaterial()->SetUniform("FinalRotations[0]", p_finalRotaion);
						}
					}
				}
			}
		}
	}

	void Chaos::Animation::Animator::SetSkeletalMesh(Resources::SkeletalMeshPtr& p_skeletalMesh)
	{
		m_skeletalMesh = p_skeletalMesh;
		SetCurrentAnimation(p_skeletalMesh->GetAnimation()[0]);
		uint32_t size = static_cast<uint32_t>(m_skeletalMesh->GetSkeleton()->GetBones().size());
		m_finalPoseData.ResizeDataContainer(size);
	}

	std::string Animation::Animator::GetSkinningMode()
	{
		if (m_skinning == SkinningMode::DUALQUAT)
			return "DUALQUAT";

		else if (m_skinning == SkinningMode::LINEAR)
			return "LINEAR";
	}

	void Chaos::Animation::Animator::SetCurrentAnimation(Resources::AnimationPtr p_animation)
	{
		m_currentAnimation = p_animation;
	}

	bool Chaos::Animation::Animator::OnKeyPressed(const Events::KeyPressedEvent & p_event)
	{
		return false;
	}

	bool Animation::Animator::OnKeyReleased(const Events::KeyReleasedEvent & p_event)
	{
		return false;
	}

	void Chaos::Animation::Animator::ChangeAnimation(E_ChaosKeyCode & p_keycode)
	{
		m_state = AnimationState::CHANGING;
	}
}