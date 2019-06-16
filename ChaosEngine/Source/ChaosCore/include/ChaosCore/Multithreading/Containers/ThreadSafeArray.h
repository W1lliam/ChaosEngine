#pragma once
#include "ChaosCore/chaos_core_pch.h"

namespace Chaos::Containers
{		
	template<typename T, size_t N>
	class ThreadSafeArray
    {
	public:
        const T& at(size_t p_pos)
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return m_data[p_pos];
        }

        void set(size_t p_pos, const T& p_data)
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_data[p_pos] = p_data;
        }

		constexpr static size_t size()
		{
            return N;
        }
	private:
        std::shared_mutex m_mutex;
        std::shared_ptr<T[]> m_data {new T[N]};
    };
}
