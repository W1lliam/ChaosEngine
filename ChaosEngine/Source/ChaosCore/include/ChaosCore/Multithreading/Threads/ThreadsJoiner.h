#pragma once

#include "ChaosCore/chaos_core_pch.h"


namespace Chaos::Threads
{
	class ThreadsJoiner
	{
	public:
		explicit ThreadsJoiner(std::vector<std::thread>& p_threads);
		~ThreadsJoiner();
    private:
        std::vector<std::thread>& m_threads;
	};
}
