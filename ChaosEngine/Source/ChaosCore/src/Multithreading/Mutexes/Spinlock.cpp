#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Multithreading/Mutexes/Spinlock.h"

namespace Chaos::Mutexes
{
    void Spinlock::lock()
    {
        while (m_lock.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();
    }

    void Spinlock::unlock()
    {
        m_lock.clear(std::memory_order_release);
    }
}