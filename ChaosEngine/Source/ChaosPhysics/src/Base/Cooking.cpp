#include "ChaosPhysics/Base/Cooking.h"

void Chaos::Physics::Cooking::InitCooking(Foundation p_foundation, physx::PxCookingParams p_cookingParam)
{
	m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *p_foundation.GetFoundation(), p_cookingParam);
}

void Chaos::Physics::Cooking::DestroyCooking() const
{
	if (m_cooking)
		m_cooking->release();
}

physx::PxCooking* Chaos::Physics::Cooking::GetCooking() const
{
	return m_cooking;
}