#pragma once

#include "ChaosCore/chaos_core_pch.h"

namespace Chaos::Mutexes
{
	class SharedSpinlock
	{
	public:
		SharedSpinlock() = default;
		~SharedSpinlock() = default;

		void lock();
		void lock_shared();
		void unlock();
		void unlock_shared();

    private:
        std::atomic<int> m_readers;
        std::atomic<bool> m_write;
	};
}