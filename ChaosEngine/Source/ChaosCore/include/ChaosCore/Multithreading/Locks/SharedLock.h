#pragma once

namespace Chaos::Locks
{
	template<class Mutex>
	class SharedLock
	{
	public:
		SharedLock() = default;
		SharedLock(Mutex& p_sharedMutex) : m_sharedMutex(&p_sharedMutex)
		{
			if(m_sharedMutex)
				m_sharedMutex->lock_shared();
		}			
		SharedLock(Mutex& p_sharedMutex, std::adopt_lock_t t) : m_sharedMutex(&p_sharedMutex)
		{}			
		SharedLock(Mutex& p_sharedMutex, std::defer_lock_t) : m_sharedMutex(&p_sharedMutex)
		{}
		~SharedLock()
		{
			if(m_sharedMutex)
				m_sharedMutex->unlock_shared();
		}

		void lock() const { if(m_sharedMutex) m_sharedMutex->lock(); }
		void unlock() const { if(m_sharedMutex) m_sharedMutex->unlock(); }

		void lock_shared() const { if(m_sharedMutex) m_sharedMutex->lock_shared(); }
		void unlock_shared() const { if(m_sharedMutex) m_sharedMutex->unlock_shared(); }

    private:
        Mutex* m_sharedMutex{ nullptr };
	};
}