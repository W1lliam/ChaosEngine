#include <iostream>
#include "ChaosPhysics/Base/VisualDebugger.h"

void Chaos::Physics::VisualDebugger::InitVisualDebugger(Foundation p_foundation)
{
	m_visualDebugger = physx::PxCreatePvd(*p_foundation.GetFoundation());
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_visualDebugger->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
}

physx::PxPvd* Chaos::Physics::VisualDebugger::GetDebugger() const
{
	return m_visualDebugger;
}