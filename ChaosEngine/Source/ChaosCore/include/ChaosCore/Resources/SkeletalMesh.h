#pragma once
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Resources/Animation.h"
#include "ChaosCore/Resources/AnimationMesh.h"
#include "ChaosCore/Resources/Skeleton.h"

namespace Chaos::Resources
{
	class SkeletalMesh : public AResource
	{
		friend class Animation;
		friend class AnimMesh;
		friend class Bone;
		friend class Skeleton;
	public:
		SkeletalMesh(Chaos::AResourceManager* p_creator, const std::string& p_name, const StringID& p_group) :
			AResource(p_creator, operator""_sid(p_name.c_str(), p_name.size()), p_group) {};

		virtual ~SkeletalMesh();

		SkeletalPtr GetSkeleton() { return m_skeleton; };
		std::vector<AnimationPtr>& GetAnimation() { return m_animation; };
		std::vector<AnimMeshPtr>& GetAnimationMesh() { return m_animMesh; };
		inline AResource::E_ResourceState   GetState()        const { return m_state.load(); };

		void AddAnimation(std::shared_ptr<SkeletalMesh> p_otherSkelMesh, AnimationPtr p_otherAnimation);
	protected:
		virtual void PreLoadImpl()    override;
		virtual void PostLoadImpl()   override;
		virtual void PreUnloadImpl()  override;
		virtual void PostUnloadImpl() override;
		virtual void PrepareImpl()    override;
		virtual void UnprepareImpl()  override;
		virtual void LoadImpl()       override;
		virtual void UnloadImpl()     override;
	private:
		bool verifySkeletonHierarchy(std::shared_ptr<SkeletalMesh> p_otherSkelMesh);
	private:
		SkeletalPtr m_skeleton;
		std::vector<AnimationPtr> m_animation;
		std::vector<AnimMeshPtr> m_animMesh;
		std::atomic<AResource::E_ResourceState>     m_state{ AResource::E_ResourceState::UNPREPARED };
		std::ifstream m_stream{};
	};

	using SkeletalMeshPtr = std::shared_ptr<SkeletalMesh>;
}