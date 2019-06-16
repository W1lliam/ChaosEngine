#pragma once
#include "ChaosCore/chaos_core_pch.h"

namespace Chaos::Mutexes
{
	class Spinlock
	{
	public:
		Spinlock() = default;
		~Spinlock() = default;

		Spinlock(const Spinlock&) = delete;
		Spinlock(Spinlock&&) = delete;
		Spinlock& operator=(const Spinlock&) = delete;
		Spinlock& operator=(Spinlock&&) = delete;

		void lock();
		void unlock();

    private:
        std::atomic_flag m_lock{ ATOMIC_FLAG_INIT };
	};
}