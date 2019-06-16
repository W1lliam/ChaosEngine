#pragma once

#include "ChaosCore/chaos_core_pch.h"

#include "ChaosCore/Multithreading/Threads/ThreadsJoiner.h"
#include "ChaosCore/Multithreading/Containers/LockFree/LFQueue.h"
#include "ChaosCore/Multithreading/Functions/FunctionWrapper.h"

namespace Chaos::Threads
{

	class ThreadPool
	{
	public:
		ThreadPool(unsigned p_desiredThreadCount = std::thread::hardware_concurrency());
		~ThreadPool();

		template<typename FunctionType>
		std::future<typename std::result_of<FunctionType()>::type> Submit(FunctionType f)
		{
			typedef typename std::result_of<FunctionType()>::type ResultType;

			std::packaged_task<ResultType()>	task(std::move(f));
			std::future<ResultType>				res(task.get_future());
			m_workQueue.push(std::move(task));
			return res;
		}

    private:
        void WorkerThread();

        Containers::LockFree::LFQueue<Functions::FunctionWrapper> m_workQueue;
        std::vector<std::thread> m_threads{};
        Threads::ThreadsJoiner m_joiner;
        std::atomic_bool m_isDone;

	};

}