#include "ChaosCore\Resources\Skeleton.h"
#include "ChaosCore/Resources/SkeletalMesh.h"
namespace Chaos
{
	Maths::Vector3 Chaos::Resources::Bone::GetWorldTranslation()
	{
		return m_transform.GetPosition();
	}

	Maths::Quaternion Chaos::Resources::Bone::GetWorldRotation()
	{
		return m_transform.GetRotationQuaternion();
	}

	Transform Chaos::Resources::Bone::GetWorldTransform()
	{
		return m_transform;
	}

	void Chaos::Resources::Bone::Prepare(std::ifstream & p_stream)
	{
		m_state.store(AResource::E_ResourceState::PREPARING);
		CHAOS_CORE_ASSERT(p_stream.is_open(), "Submesh can only load from an opened stream")
			AssetType type;
		p_stream.read(reinterpret_cast<char*>(&type), 1);
		if (type != AssetType::E_ASSETTYPE_BONE)
		{
			CHAOS_CORE_ERROR("Asset type is different from expected type");
			p_stream.close();
			m_state.store(AResource::E_ResourceState::UNPREPARED);
			throw std::exception((std::string("Asset " + m_name) + " is not valid").c_str());
		}
		m_state.store(AResource::E_ResourceState::PREPARED);
	}

	void Chaos::Resources::Bone::Load(std::ifstream & p_stream, const uint32_t& p_id)
	{
		m_state.store(AResource::E_ResourceState::LOADING);
		CHAOS_CORE_ASSERT(p_stream.is_open(), "Asset file has not been opened");

		uint32_t blockSize;
		p_stream.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));

		getline(p_stream, m_name, '\0');
		getline(p_stream, m_parentName, '\0');

		Maths::Quaternion rot;
		Maths::Vector3 pos;
		p_stream.read(reinterpret_cast<char*>(&rot.GetX()), sizeof(float));
		p_stream.read(reinterpret_cast<char*>(&rot.GetY()), sizeof(float));
		p_stream.read(reinterpret_cast<char*>(&rot.GetZ()), sizeof(float));
		p_stream.read(reinterpret_cast<char*>(&rot.GetRealValue()), sizeof(float));

		p_stream.read(reinterpret_cast<char*>(&pos.x), sizeof(float));
		p_stream.read(reinterpret_cast<char*>(&pos.y), sizeof(float));
		p_stream.read(reinterpret_cast<char*>(&pos.z), sizeof(float));

		m_transform = Transform(pos, rot);
		m_id = p_id;
		m_state.store(AResource::E_ResourceState::LOADED);
	}

	void Chaos::Resources::Bone::PostLoad()
	{
	}

	void Chaos::Resources::Bone::Unload()
	{
		m_state.store(AResource::E_ResourceState::UNPREPARED);
	}

	Chaos::Resources::BonePtr Resources::Skeleton::GetBoneByName(const std::string & p_name)
	{
		for (size_t i = 0; i < m_bones.size(); ++i)
		{
			if (m_bones[i]->GetName() == p_name)
			{
				return m_bones[i];
			}
		}
        return nullptr;
	}

	std::string Resources::Skeleton::GetParentsName(const std::string & p_string)
	{
		for (size_t i = 0; i < m_bones.size(); ++i)
		{
			if (m_bones[i]->GetName() == p_string)
			{
				return m_bones[i]->GetParentName();
			}
		}
        return {};
	}

	std::vector<Transform> Resources::Skeleton::GetWorldTposTransform()
	{
		std::vector<Transform> temp;
		temp.reserve(m_bones.size());
		for (size_t i = 0; i < m_bones.size(); ++i)
		{
			temp.push_back(m_bones[i]->GetWorldTransform());
		}
		return temp;
	}
	std::unordered_map<std::string, Maths::Vector3> Resources::Skeleton::GetWorldTposTranslation()
	{
		std::unordered_map<std::string, Maths::Vector3> temp;
		temp.reserve(m_bones.size());
		for (BonePtr& bone : m_bones)
		{
			temp[bone->GetName()] = bone->GetWorldTranslation();
		}
		return temp;
	}

	std::unordered_map<std::string, Maths::Quaternion> Resources::Skeleton::GetWorldTposRotation()
	{
		std::unordered_map<std::string, Maths::Quaternion> temp;
		temp.reserve(m_bones.size());
		for (BonePtr& bone : m_bones)
		{
			temp[bone->GetName()] = bone->GetWorldRotation();
		}
		return temp;
	}
}