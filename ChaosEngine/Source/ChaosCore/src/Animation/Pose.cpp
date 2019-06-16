#include "ChaosCore\Animation\Pose.h"
namespace Chaos
{
	void Chaos::Pose::AddTransform(const std::string & p_boneName, const Transform & p_transform)
	{
		m_currentPoseTransform.emplace(p_boneName, p_transform);
	}

	void Chaos::Pose::AddTranslationAndRotation(const std::string & p_boneName, const Maths::Vector3 & p_translation, const Maths::Quaternion & p_rotation)
	{
		m_currentPoseTransform.emplace(p_boneName, Transform(p_translation, p_rotation));
	}

	Transform Chaos::Pose::GetLocalTransform(const std::string & p_boneName)
	{
		return m_currentPoseTransform[p_boneName];
	}

	Maths::Vector3 Chaos::Pose::GetLocalTranslation(const std::string & p_boneName)
	{
		return m_currentPoseTransform[p_boneName].GetPosition();
	}

	Maths::Quaternion Chaos::Pose::GetLocalRotation(const std::string & p_boneName)
	{
		return m_currentPoseTransform[p_boneName].GetRotationQuaternion();
	}

	Transform Chaos::Pose::GetWorldTransform(const std::string & p_boneName)
	{
		Transform ParentTransform;
		std::string parentName = m_skeleton->GetParentsName(p_boneName);
		if (parentName == "NULL")
		{
			return m_currentPoseTransform[p_boneName];
		}
		else
		{
			ParentTransform = GetWorldTransform(parentName);
		}
		return m_currentPoseTransform[p_boneName].TransformFromParent(ParentTransform);
	}

	Maths::Vector3 Chaos::Pose::GetWorldTranslation(const std::string& p_boneName)
	{
		Transform CurrentWorldTransform = GetWorldTransform(p_boneName);
		return CurrentWorldTransform.GetPosition();
	}

	Maths::Quaternion Chaos::Pose::GetWorldRotation(const std::string & p_boneName)
	{
		Transform CurrentWorldTransform = GetWorldTransform(p_boneName);
		Maths::Quaternion quat = CurrentWorldTransform.GetRotationQuaternion();
		return quat;
	}

	std::vector<Transform> Pose::GetAnimWorldTransform()
	{
		std::vector<Transform> temp;
		temp.reserve(m_skeleton->GetBones().size());
		for (size_t i = 0; i < m_skeleton->GetBones().size(); ++i)
		{
			temp.push_back(GetWorldTransform(m_skeleton->GetBones()[i]->GetName()));
		}
		return temp;
	}
	/*
	std::unordered_map<std::string, Maths::Vector3> Pose::GetWorldAnimTranslations()
	{
		std::unordered_map<std::string, Maths::Vector3> temp;
		temp.reserve(m_skeleton->GetBones().size());
		for (Chaos::Resources::BonePtr bone : m_skeleton->GetBones())
		{
			temp[bone->GetName()] = GetWorldTranslation(bone->GetName());
		}
		return temp;
	}
	std::unordered_map<std::string, Maths::Quaternion> Pose::GetWorldAnimRotations()
	{
		std::unordered_map<std::string, Maths::Quaternion> temp;
		temp.reserve(m_skeleton->GetBones().size());
		for (Chaos::Resources::BonePtr bone : m_skeleton->GetBones())
		{
			temp[bone->GetName()] = GetWorldRotation(bone->GetName());
		}
		return temp;
	}*/
}