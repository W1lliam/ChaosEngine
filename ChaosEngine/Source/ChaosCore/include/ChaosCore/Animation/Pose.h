#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <ChaosMaths.h>
#include "ChaosCore/Resources/Skeleton.h"
#include "ChaosCore/Transform.h"
namespace Chaos
{
	class Pose
	{
	public:
		Pose() = default;
		~Pose() = default;

		void AddTransform(const std::string& p_boneName, const Transform& p_transform);
		void AddTranslationAndRotation(const std::string& p_boneName, const Maths::Vector3& p_translation,
									   const Maths::Quaternion& p_rotation);

		inline  void SetSkeletonPtr(Chaos::Resources::SkeletalPtr p_skeleton) { m_skeleton = p_skeleton; };
		
		Transform GetLocalTransform(const std::string& p_boneName);
		Maths::Vector3 GetLocalTranslation(const std::string& p_boneName);
		Maths::Quaternion GetLocalRotation(const std::string& p_boneName);
		
		Transform GetWorldTransform(const std::string& p_boneName);
		Maths::Vector3 GetWorldTranslation(const std::string& p_boneName);
		Maths::Quaternion GetWorldRotation(const std::string& p_boneName);
		//World Transform is returned in bone_order
		std::vector<Transform> GetAnimWorldTransform();
		
	private:
		Resources::SkeletalPtr m_skeleton;
		std::unordered_map<std::string, Transform> m_currentPoseTransform;
	};
}