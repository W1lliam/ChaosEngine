#pragma once
#include <map>
#include <string>

namespace Chaos
{
	class Collider;
	class CollisionSetting
	{
	public:
		CollisionSetting();
		~CollisionSetting();

		void AddCollisionLayer(std::string p_layerName);
		void DeleteCollisionLayer(std::string p_layerName);
		void SetBlockCollision(std::string p_layerName, bool p_enabled);
		void SetUpCollisionFilterToPhysX();
		void SetUpOwnerMask(std::string p_mask);
		void SetUpOnwerCollider(Collider* p_collider);

		std::vector<std::pair<std::string, bool>>& GetCollisionSetting();
		std::string& GetOwnerMask();

	private:
		std::vector<std::pair<std::string, bool>> m_collisionSetting;
		std::string m_ownerType;
		Collider* m_collider = nullptr;
	};
}