#include "ChaosPhysics/Friction/Friction.h"

Chaos::Physics::Friction::Friction(float p_staticFriction, float p_dynamicFriction, float p_restitution)
{
	m_material = ElementCreator::Get().GetPxPhysics()->createMaterial(p_staticFriction, p_dynamicFriction, p_restitution);
}

Chaos::Physics::Friction::~Friction() = default;

physx::PxMaterial * Chaos::Physics::Friction::GetMaterial() const
{
	return m_material;
}