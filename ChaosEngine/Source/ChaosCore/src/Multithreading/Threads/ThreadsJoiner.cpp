#include "ChaosCore/chaos_core_pch.h"

#include "ChaosCore/Multithreading/Threads/ThreadsJoiner.h"

namespace	Chaos::Threads
{
    ThreadsJoiner::ThreadsJoiner(std::vector<std::thread>& p_threads) : m_threads(p_threads)
    {
    }

    ThreadsJoiner::~ThreadsJoiner()
    {
        for (unsigned long i = 0; i < m_threads.size(); ++i)
        {
            if (m_threads[i].joinable())
                m_threads[i].join();
        }
    }
}