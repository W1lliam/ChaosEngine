#include "ChaosCore/Physics/CollisionFiltering.h"
#include "ChaosCore/Physics/Collider.h"
#include <physx/include/PxPhysicsAPI.h>

Chaos::CollisionSetting::CollisionSetting()
{
}

Chaos::CollisionSetting::~CollisionSetting()
{
}

void Chaos::CollisionSetting::AddCollisionLayer(std::string p_layerName)
{
	bool flag = false;
	auto name = std::find_if(m_collisionSetting.begin(), m_collisionSetting.end(),
		[&p_layerName, &flag](const std::pair<std::string, bool>& element)
	{
		std::string layername = element.first;
		flag = layername == p_layerName;
		return flag;
	});
	if (!flag)
	{
		m_collisionSetting.emplace_back(p_layerName, false);
	}
}

void Chaos::CollisionSetting::DeleteCollisionLayer(std::string p_layerName)
{
 auto it = std::remove_if(m_collisionSetting.begin(), m_collisionSetting.end(),
			[p_layerName](const std::pair<std::string, bool>& element)
			{
				std::string layerName = element.first;
				return layerName == p_layerName;
			});
}

void Chaos::CollisionSetting::SetBlockCollision(std::string p_layerName, bool p_enabled)
{
	auto name = std::find_if(m_collisionSetting.begin(), m_collisionSetting.end(),
		[&p_layerName](std::pair<std::string, bool>& element)
	{
		std::string layername = element.first;
		return layername == p_layerName;
	});
	name->second = p_enabled;
}

void Chaos::CollisionSetting::SetUpCollisionFilterToPhysX()
{
	physx::PxU32 mask;
	physx::PxU32 toBlock;
	for (int i = 0; i < m_collisionSetting.size(); ++i)
	{
		if (m_collisionSetting[i].first == m_ownerType)
			mask = (1 << i);

		if (m_collisionSetting[i].second)
			toBlock |= (1 << i);
	}

	physx::PxFilterData filterData;
	filterData.word0 = mask;
	filterData.word1 = toBlock;
	m_collider->GetPhysicsShape()->GetShape()->setSimulationFilterData(filterData);
}

void Chaos::CollisionSetting::SetUpOwnerMask(std::string p_mask)
{
	m_ownerType = p_mask;
}

void Chaos::CollisionSetting::SetUpOnwerCollider(Collider * p_collider)
{
	m_collider = p_collider;
}

std::vector<std::pair<std::string, bool>>& Chaos::CollisionSetting::GetCollisionSetting()
{
	return m_collisionSetting;
}

std::string & Chaos::CollisionSetting::GetOwnerMask()
{
	return m_ownerType;
}