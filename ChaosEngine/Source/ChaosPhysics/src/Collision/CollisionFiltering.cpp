#include "ChaosPhysics\Collision\CollisionFiltering.h"

Chaos::Physics::CollisionFilter::CollisionFilter()
{
}

void Chaos::Physics::CollisionFilter::UpdateFilter(int p_index, bool p_block)
{
}

physx::PxFilterData Chaos::Physics::CollisionFilter::GetFilterData()
{
	return physx::PxFilterData();
}
