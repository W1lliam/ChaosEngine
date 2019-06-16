#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Multithreading/Mutexes/SharedSpinlock.h"

namespace Chaos::Mutexes
{
    void SharedSpinlock::lock()
    {
        while (m_write.exchange(true, std::memory_order_acquire))
            std::this_thread::yield();

        while (m_readers != 0)
            std::this_thread::yield();
    }

    void SharedSpinlock::lock_shared()
    {
        while (true)
        {
            while (m_write.load())
                std::this_thread::yield();

            m_readers.fetch_add(1, std::memory_order_acquire);

            //If write locked during operation, retry read
            if (m_write.load())
                m_readers.fetch_sub(1, std::memory_order_release);
            else
                return;
        }
    }

    void SharedSpinlock::unlock()
    {
        m_write.store(false, std::memory_order_release);
    }

    void SharedSpinlock::unlock_shared()
    {
        m_readers.fetch_sub(1, std::memory_order_release);
    }
}