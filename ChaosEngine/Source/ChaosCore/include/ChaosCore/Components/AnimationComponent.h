#pragma once
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/Animation/Animator.h"

#ifdef CHAOS_EDITOR
namespace ChaosEditor { class AnimationComponentGUI; }
#endif
namespace Chaos
{
	class AnimationComponent : public IComponent
	{
		CHAOS_COMPONENT
#ifdef CHAOS_EDITOR
			friend class ::ChaosEditor::AnimationComponentGUI;
#endif
	public:
		AnimationComponent(COMPONENT_INIT_PARAMS, Resources::SkeletalMeshPtr p_skelMesh = {});
		
		void SetSkeletalMesh(Resources::SkeletalMeshPtr p_skelMesh);
		const Resources::SkeletalMeshPtr GetSkeletalMesh() const;
		const Animation::AnimatorPtr GetAnimator() const;
		virtual bool        Save(WriteStream& p_stream) override;
		static  IComponent& Load(ReadStream& p_stream, const EntityID& p_entityID);

		void PlayAnimation() const;
		void PauseAnimation() const;
		void StopAnimation() const;

		void SetSkinningMode(Animation::SkinningMode p_mode);

	private:
		Resources::SkeletalMeshPtr m_skeletalMesh;
		Animation::AnimatorPtr m_animator;
	};
}