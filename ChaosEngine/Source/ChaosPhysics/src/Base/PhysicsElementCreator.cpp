#include "ChaosPhysics/Base/PhysicsElementCreator.h"
#include <Windows.h>
#include <ChaosDebug.h>

namespace Chaos
{
    Physics::ElementCreator& Physics::ElementCreator::Get()
    {
#ifdef CHAOS_PLATFORM_WINDOWS
        typedef ElementCreator& (*GetSingleton)();
        HMODULE mod = GetModuleHandle(TEXT("ChaosEngine.dll"));
        CHAOS_CORE_ASSERT(mod, "ChaosEngine.dll is missing");
        GetSingleton getSingleton = (GetSingleton) ::GetProcAddress(mod, "GetElementCreator");
        return getSingleton();
#else
        static ElementCreator instance;
        return instance;
#endif
    }

    physx::PxPhysics * Chaos::Physics::ElementCreator::GetPxPhysics() const
    {
        return m_physics;
    }

    physx::PxDefaultCpuDispatcher* Chaos::Physics::ElementCreator::GetCpuDispatcher() const
    {
        return m_cpuDispatcher;
    }

	physx::PxCooking * Physics::ElementCreator::GetPxCooking() const
	{
		return m_cooking.GetCooking();
	}

    Chaos::Physics::ElementCreator::ElementCreator()
    {
        InitPhysX();
    }

    Chaos::Physics::ElementCreator::~ElementCreator()
    {
        DestroyPhysX();
        m_foundation.DestroyFoundation();
    }

    void Chaos::Physics::ElementCreator::InitPhysX()
    {
        m_foundation.InitFoundation();
        physx::PxTolerancesScale scale = physx::PxTolerancesScale();
        m_visualDebugger.InitVisualDebugger(m_foundation);
        InitPhysics(scale);
        m_cooking.InitCooking(m_foundation, physx::PxCookingParams(scale));
        InitCpuThreads(4);
    }

    void Chaos::Physics::ElementCreator::InitPhysics(physx::PxTolerancesScale& p_tolerance)
    {
        m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation.GetFoundation(), p_tolerance, true, m_visualDebugger.GetDebugger());
    }

    void Chaos::Physics::ElementCreator::InitCpuThreads(int p_numberofThreadsForSimuation)
    {
        m_cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(p_numberofThreadsForSimuation);
    }

    void Chaos::Physics::ElementCreator::DestroyPhysX() const
    {
        m_cpuDispatcher->release();
        m_cooking.DestroyCooking();
        m_physics->release();
        physx::PxPvdTransport* transport = m_visualDebugger.GetDebugger()->getTransport();
        m_visualDebugger.GetDebugger()->release();
        transport->release();
    }
}