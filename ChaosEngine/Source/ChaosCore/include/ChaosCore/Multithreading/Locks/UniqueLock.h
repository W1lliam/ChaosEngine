#pragma once

namespace Chaos::Locks
{
	template<class Mutex>
	class UniqueLock
	{
	public:
		UniqueLock() = default;
		explicit UniqueLock(Mutex& p_mutex) : m_mutex(&p_mutex)
		{
			if(m_mutex)
				m_mutex->lock();
		}
		explicit UniqueLock(Mutex& p_mutex, std::adopt_lock_t t) : m_mutex(&p_mutex) {}
		explicit UniqueLock(Mutex& p_mutex, std::defer_lock_t t) : m_mutex(&p_mutex) {}

		UniqueLock(UniqueLock&& p_other) noexcept : m_mutex(std::move(p_other.m_mutex))
		{
		}
		UniqueLock& operator=(const UniqueLock& p_other) noexcept
		{
			m_mutex = std::move(p_other.m_mutex);
			return *this;
		}
		UniqueLock& operator=(UniqueLock&& p_other) noexcept
		{
			m_mutex = std::move(p_other.m_mutex);
			return *this;
		}
		~UniqueLock()
		{
			if(m_mutex)
				m_mutex->unlock();
			m_mutex = nullptr;
		}
			
		void lock() const { if(m_mutex) m_mutex->lock(); }
		void unlock() const { if(m_mutex) m_mutex->unlock(); }

    private:
        Mutex* m_mutex{ nullptr };
	};
}
