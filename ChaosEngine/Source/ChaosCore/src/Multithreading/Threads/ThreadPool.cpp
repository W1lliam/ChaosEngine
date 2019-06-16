#include "ChaosCore/chaos_core_pch.h"

#include "ChaosCore/Multithreading/Threads/ThreadPool.h"
#include "ChaosCore/Multithreading/Functions/FunctionWrapper.h"

namespace Chaos::Threads
{
    void ThreadPool::WorkerThread()
    {
        while (!m_isDone)
        {
            std::unique_ptr<Functions::FunctionWrapper> task = m_workQueue.pop();
            if (task)
                (*task)();
            else
                std::this_thread::yield();
        }
    }

    ThreadPool::ThreadPool(unsigned p_desiredThreadCount) : m_joiner(m_threads), m_isDone(false)
    {
        unsigned const threadCount = std::min(std::thread::hardware_concurrency(), p_desiredThreadCount);
        try
        {
            for (unsigned i = 0; i < threadCount; ++i)
            {
                m_threads.emplace_back(&ThreadPool::WorkerThread, this);
            }
        }
        catch (...)
        {
            m_isDone = true;
            throw;
        }
    }


    ThreadPool::~ThreadPool()
    {
        m_isDone = true;
    }
}