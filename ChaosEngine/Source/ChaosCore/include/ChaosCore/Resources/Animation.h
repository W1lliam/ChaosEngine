#pragma once
#include <ChaosMaths.h>
#include <unordered_map>
#include <vector>
#include "ChaosCore/Resources/AResource.h"
#include "Skeleton.h"
namespace Chaos::Resources
{
	struct AnimationKeyFrame
	{
		double timeStamp;
		std::vector<std::pair<std::string, Maths::Vector3>> PosAtTime;
		std::vector<std::pair<std::string, Maths::Quaternion>> RotAtTime;
	};

	class Animation
	{
		friend class SkeletalMesh;
	public:
		Animation();
		~Animation();

		inline std::vector<AnimationKeyFrame>& GetAnimNodePerFrame() { return m_animationNodePerFrame; };
		inline const double& GetTicksPerSecond() { return m_tickPerSecond; };
		inline const double& GetDuration() { return m_duration; };
		inline const std::string& GetName() { return m_animationName; };

	protected:
		void Prepare(std::ifstream& p_stream);
		void Load(std::ifstream& p_stream, SkeletalPtr p_skeleton);
		void PostLoad();
		void Unload();
	private:
		std::vector<std::pair<std::string, Maths::Quaternion>> FillEmptyRotationAnimData(SkeletalPtr p_skeleton, uint32_t& size);
		std::vector<std::pair<std::string, Maths::Vector3>> FillEmptyPositionAnimData(SkeletalPtr p_skeleton, uint32_t& size);
	private:
		std::vector<AnimationKeyFrame> m_animationNodePerFrame;
		std::string m_animationName;
		double m_tickPerSecond;
		double m_duration;

		std::atomic<AResource::E_ResourceState>     m_state{ AResource::E_ResourceState::UNPREPARED };
	};

	using AnimationPtr = std::shared_ptr<Animation>;
}