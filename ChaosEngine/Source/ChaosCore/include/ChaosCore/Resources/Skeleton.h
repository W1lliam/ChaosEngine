#pragma once
#include <ChaosMaths.h>
#include <string>
#include <memory>
#include <vector>
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Transform.h"
namespace Chaos::Resources
{
	class Bone
	{
		friend class SkeletalMesh;
		friend class Skeleton;
	public:
		Bone() {};
		~Bone() = default;

		inline const std::string&			GetName()				{ return m_name; };
		inline const std::string&			GetParentName()			{ return m_parentName; };
		inline const uint32_t&				GetBoneID()				{ return m_id; };
		inline AResource::E_ResourceState   GetState()        const { return m_state.load(); }
		
		inline void							SetName(const std::string& p_name) { m_name = p_name; };
		inline void							SetID(const uint32_t& p_id) { m_id = p_id; };
		Maths::Vector3 GetWorldTranslation();
		Maths::Quaternion GetWorldRotation();
		Transform GetWorldTransform();

	protected:
		void Prepare(std::ifstream& p_stream);
		void Load(std::ifstream& p_stream, const uint32_t& p_id);
		void PostLoad();
		void Unload();

	private:
		std::string m_name;
		std::string m_parentName;
		uint32_t m_id;
		Transform m_transform;
		std::atomic<AResource::E_ResourceState>     m_state{ AResource::E_ResourceState::UNPREPARED };
	};
	using BonePtr = std::shared_ptr<Bone>;
	
	class Skeleton
	{
		friend class SkeletalMesh;
	public:
		Skeleton() {};
		virtual ~Skeleton() = default;
		BonePtr GetBoneByName(const std::string& p_name);
		inline  const std::vector<BonePtr>&	GetBones()												{ return m_bones; };
			    std::string			        GetParentsName(const std::string& p_string);
		std::vector<Transform> GetWorldTposTransform();
		std::unordered_map<std::string, Maths::Vector3> GetWorldTposTranslation();
		std::unordered_map<std::string, Maths::Quaternion> GetWorldTposRotation();

	private:

		std::vector<BonePtr> m_bones;
	};
	using SkeletalPtr = std::shared_ptr<Skeleton>;
}