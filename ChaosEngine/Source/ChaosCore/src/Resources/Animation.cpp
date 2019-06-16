#include "ChaosCore\Resources\Animation.h"
#include "ChaosCore/Resources/SkeletalMesh.h"
namespace Chaos
{
	std::unordered_map<std::string, Maths::Vector3> GetPrevPosMap(const std::map<double, std::unordered_map<std::string, Maths::Vector3>>& p_posMap, double& p_key)
	{
		auto itLow = p_posMap.lower_bound(p_key);
		if (itLow != p_posMap.begin())
		{
			itLow--;
			return itLow->second;
		}
		else
			return itLow->second;
	}
	std::unordered_map<std::string, Maths::Quaternion> GetPrevRotMap(const std::map<double, std::unordered_map<std::string, Maths::Quaternion>>& p_rotMap, double& p_key)
	{
		auto itLow = p_rotMap.lower_bound(p_key);
		if (itLow != p_rotMap.begin())
		{
			itLow--;
			return itLow->second;
		}
		else
			return itLow->second;
	}
}

namespace Chaos
{
	Chaos::Resources::Animation::Animation()
	{
	}

	Chaos::Resources::Animation::~Animation()
	{
	}
	void Chaos::Resources::Animation::Prepare(std::ifstream& p_stream)
	{
		m_state.store(AResource::E_ResourceState::PREPARING);
		CHAOS_CORE_ASSERT(p_stream.is_open(), "Submesh can only load from an opened stream")
			AssetType type;
		p_stream.read(reinterpret_cast<char*>(&type), 1);
		if (type != AssetType::E_ASSETTYPE_ANIMATION)
		{
			CHAOS_CORE_ERROR("Asset type is different from expected type");
			p_stream.close();
			m_state.store(AResource::E_ResourceState::UNPREPARED);
			throw std::exception((std::string("AnimationMesh Asset") + " is not valid").c_str());
		}
		m_state.store(AResource::E_ResourceState::PREPARED);
	}

	void Chaos::Resources::Animation::Load(std::ifstream& p_stream, SkeletalPtr p_skeleton)
	{
		m_state.store(AResource::E_ResourceState::LOADING);
		CHAOS_CORE_ASSERT(p_stream.is_open(), "Asset file has not been opened");

		uint32_t blockSize;
		p_stream.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));

		//TODO: Either use name or remove from asset file
		getline(p_stream, m_animationName, '\0');

		p_stream.read(reinterpret_cast<char*>(&m_duration), sizeof(m_duration));
		p_stream.read(reinterpret_cast<char*>(&m_tickPerSecond), sizeof(m_tickPerSecond));

		uint32_t numberOfChannel = 0;
		p_stream.read(reinterpret_cast<char*>(&numberOfChannel), sizeof(numberOfChannel));

		std::map<uint32_t, std::vector<std::pair<std::string, Maths::Vector3>>> posMap;
		std::map<uint32_t, std::vector<std::pair<std::string, Maths::Quaternion>>> rotMap;
	
		bool filled = false;
		for (uint32_t channel = 0; channel < numberOfChannel; ++channel)
		{		
			std::string nodeName{};
			uint32_t numberOfpositionKey = 0;
			uint32_t numberOfrotationKey = 0;

			//retrieve the name of bone
			getline(p_stream, nodeName, '\0');
			uint32_t id = p_skeleton->GetBoneByName(nodeName)->GetBoneID();
			p_stream.read(reinterpret_cast<char*>(&numberOfpositionKey), sizeof(numberOfpositionKey));
			if (!filled)
			{
				for (uint32_t i = 0; i < numberOfpositionKey; ++i)
				{
					posMap[i] = FillEmptyPositionAnimData(p_skeleton, numberOfChannel);
					rotMap[i] = FillEmptyRotationAnimData(p_skeleton, numberOfChannel);
				}
				filled = true;
			}
			for (uint32_t positionKey = 0; positionKey < numberOfpositionKey; ++positionKey)
			{
				double time = 0;
				Maths::Vector3 pos;
				p_stream.read(reinterpret_cast<char*>(&time), sizeof(time));
				p_stream.read(reinterpret_cast<char*>(&pos.x), sizeof(pos.x));
				p_stream.read(reinterpret_cast<char*>(&pos.y), sizeof(pos.y));
				p_stream.read(reinterpret_cast<char*>(&pos.z), sizeof(pos.z));
				posMap[static_cast<uint32_t>(time)].at(id).first = nodeName;
				posMap[static_cast<uint32_t>(time)].at(id).second = pos;
			}
			p_stream.read(reinterpret_cast<char*>(&numberOfrotationKey), sizeof(numberOfrotationKey));
			for (uint32_t rotationKey = 0; rotationKey < numberOfrotationKey; ++rotationKey)
			{
				double time = 0;
				Maths::Vector4 quat;
				p_stream.read(reinterpret_cast<char*>(&time), sizeof(time));
				p_stream.read(reinterpret_cast<char*>(&quat.x), sizeof(quat.x));
				p_stream.read(reinterpret_cast<char*>(&quat.y), sizeof(quat.y));
				p_stream.read(reinterpret_cast<char*>(&quat.z), sizeof(quat.z));
				p_stream.read(reinterpret_cast<char*>(&quat.w), sizeof(quat.w));
				rotMap[static_cast<uint32_t>(time)].at(id).first = nodeName;
				rotMap[static_cast<uint32_t>(time)].at(id).second = quat;
			}
		}

		for (uint32_t i = 1; i < posMap.size(); ++i)
		{
			for (uint32_t j = 0; j < posMap[0].size(); ++j)
			{
				if (posMap[i].at(j).second == Maths::Vector3(INT_MIN, INT_MIN, INT_MIN))
				{
					posMap[i].at(j).second = posMap[i - 1].at(j).second;
				}
				Maths::Quaternion tocompare = rotMap[i].at(j).second;
				if(tocompare.GetX() == 0.f && tocompare.GetY() == 0.f && tocompare.GetZ() == 0.f && tocompare.GetRealValue() == 0.f)
				{
					rotMap[i].at(j).second = rotMap[i - 1].at(j).second;
				}
			}
		}
		for (auto const& imap : posMap)
		{
			AnimationKeyFrame keyframe;
			keyframe.timeStamp = static_cast<double>(imap.first);
			keyframe.PosAtTime = posMap[static_cast<uint32_t>(keyframe.timeStamp)];
			keyframe.RotAtTime = rotMap[static_cast<uint32_t>(keyframe.timeStamp)];
			m_animationNodePerFrame.push_back(keyframe);
		}
	}

	void Chaos::Resources::Animation::PostLoad()
	{
	}

	void Chaos::Resources::Animation::Unload()
	{
		m_state.store(AResource::E_ResourceState::UNLOADING);
		m_animationNodePerFrame.clear();
		m_state.store(AResource::E_ResourceState::UNPREPARED);
	}
	std::vector<std::pair<std::string, Maths::Quaternion>> Resources::Animation::FillEmptyRotationAnimData(SkeletalPtr p_skeleton, uint32_t& p_size)
	{
		std::vector<std::pair<std::string, Maths::Quaternion>> toReturn;
		toReturn.reserve(p_skeleton->GetBones().size());
			for (uint32_t i = 0; i < p_skeleton->GetBones().size(); ++i)
			{
				toReturn.emplace_back(p_skeleton->GetBones()[i]->GetName(), Maths::Quaternion(0.f, 0.f, 0.f, 0.f));
			}
		return toReturn;
	}
	std::vector<std::pair<std::string, Maths::Vector3>> Resources::Animation::FillEmptyPositionAnimData(SkeletalPtr p_skeleton, uint32_t& p_size)
	{
		std::vector<std::pair<std::string, Maths::Vector3>> toReturn;
		toReturn.reserve(p_skeleton->GetBones().size());
		for (uint32_t i = 0; i < p_skeleton->GetBones().size(); ++i)
		{
			toReturn.emplace_back(p_skeleton->GetBones()[i]->GetName(), Maths::Vector3(INT_MIN, INT_MIN, INT_MIN));
		}
		return toReturn;
	}
}