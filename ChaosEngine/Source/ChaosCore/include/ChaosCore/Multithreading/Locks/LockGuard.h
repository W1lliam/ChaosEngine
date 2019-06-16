#pragma once

namespace Chaos::Locks
{
	template<class Mutex>
	class LockGuard
	{
	public:
		explicit LockGuard(Mutex& p_mutex) : m_mutex(p_mutex)
		{
			m_mutex.lock();
		}
        explicit LockGuard(Mutex& p_mutex, std::adopt_lock_t t) : m_mutex(p_mutex)
		{}
		~LockGuard()
		{
			m_mutex.unlock();
		}

    private:
        Mutex& m_mutex;
	};
}