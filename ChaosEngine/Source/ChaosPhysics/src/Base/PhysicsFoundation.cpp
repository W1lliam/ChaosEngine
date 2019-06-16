#include "ChaosPhysics/Base/PhysicsFoundation.h"

Chaos::Physics::Foundation::~Foundation()
{
}

physx::PxFoundation * Chaos::Physics::Foundation::GetFoundation() const
{
	return m_foundation;
}

void Chaos::Physics::Foundation::InitFoundation()
{
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);
}

void Chaos::Physics::Foundation::DestroyFoundation()
{
	if (m_foundation)
	{
		m_foundation->release();
		m_foundation = nullptr;
	}
}