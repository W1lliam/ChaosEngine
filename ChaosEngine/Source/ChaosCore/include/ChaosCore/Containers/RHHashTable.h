#pragma once
#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"
#include "ChaosCore/Utils/CRC.h"
#include "ChaosDebug.h"

namespace Chaos
{
    template<typename Key,typename Value>
    class RHHashTable
    {
    public:
        RHHashTable(uint64_t p_count) : m_allocator{sizeof(Value) * p_count, sizeof(Value), alignof(Value)}, m_bucketsCount { p_count }, m_maxProbe{ p_count }
        {
            Init();
        }

        void Init()
        {
            if(!m_buckets)
                m_buckets = new Bucket<Value>[m_bucketsCount];
            memset(m_buckets, 0, sizeof(Bucket<Value>) * m_bucketsCount);
            m_bucketsUsed = 0;
        }

        void Close()
        {
            m_allocator.Clear();
            delete[] m_buckets;
            m_buckets = nullptr;
        }

        void Clear()
        {
            m_allocator.Clear();
            memset(m_buckets, 0, sizeof(Bucket<Value>) * m_bucketsCount);
            m_bucketsUsed = 0;
        }

        void Insert(const Key& p_key, const Value& p_value)
        {
            if (m_bucketsUsed == m_bucketsCount)
                throw std::overflow_error("Map is full");

            Hash    hash    = HashFunction(p_key);
            Value*  data    = static_cast<Value*>(m_allocator.Allocate(sizeof(Value), alignof(Value)));
			data			= new (data) Value(p_value);

            uint64_t initIndex              = hash % m_bucketsCount;
            uint64_t currentProbeDistance   = 0;

            for (uint64_t i = 0; i < m_maxProbe; ++i)
            {
                uint64_t currentIndex = (initIndex + i) % m_bucketsCount;
                if (m_buckets[currentIndex].data == nullptr)
                {
                    m_buckets[currentIndex].hash = hash;
                    m_buckets[currentIndex].data = data;
                    ++m_bucketsUsed;
                    return;
                }
                else
                {
                    uint64_t probeDistance = DistanceToInitIndex(currentIndex);
                    if (currentProbeDistance > probeDistance)
                    {
                        std::swap(hash, m_buckets[currentIndex].hash);
                        std::swap(data, m_buckets[currentIndex].data);
                        currentProbeDistance = probeDistance;
                    }
                }
                ++currentProbeDistance;
            }
            throw std::overflow_error("Insertion exceeded max probing distance");
        }

        void Remove(const Key& p_key)
        {
            Hash        hash                    = HashFunction(p_key);
            uint64_t    initIndex               = hash % m_bucketsCount;
            uint64_t    currentProbeDistance    = 0;
            uint64_t    currentIndex            = initIndex;

            for (uint64_t i = 0; i < m_bucketsCount; ++i)
            {
                currentIndex = (initIndex + i) % m_bucketsCount;
                uint64_t probeDistance = DistanceToInitIndex(currentIndex);
                if (m_buckets[currentIndex].data == nullptr || i > probeDistance) 
                {
                     throw std::out_of_range("Entry not found");
                }
                if (m_buckets[currentIndex].hash == hash) 
                {
                    m_allocator.Free(m_buckets[currentIndex].data);
                    --m_bucketsUsed;
                    break;
                }
            }

            for (uint64_t i = 1; i < m_bucketsCount; ++i)
            {
                uint64_t previousIndex  = (currentIndex + i - 1)    % m_bucketsCount;
                uint64_t swapIndex      = (currentIndex + i)        % m_bucketsCount;

                if (m_buckets[swapIndex].data == nullptr)
                {
                    m_buckets[previousIndex].data = nullptr;
                    return;
                }

                uint64_t distance = DistanceToInitIndex(swapIndex);
                if (distance == 0)
                {
                    m_buckets[previousIndex].data = nullptr;
                    return;
                }

                m_buckets[previousIndex].data = m_buckets[swapIndex].data;
                m_buckets[previousIndex].hash = m_buckets[swapIndex].hash;
            }
        }

		bool Contains(const Key& p_key)
		{
			Hash        hash = HashFunction(p_key);
			uint64_t    initIndex = hash % m_bucketsCount;

			for (uint64_t i = 0; i < m_maxProbe; ++i)
			{
				uint64_t currentIndex = (initIndex + i) % m_bucketsCount;

				if (m_buckets[currentIndex].data == nullptr)
				{
					return false;
				}

				uint64_t currentProbeDistance = DistanceToInitIndex(currentIndex);
				if (i > currentProbeDistance)
					return false;
				if (m_buckets[currentIndex].hash == hash)
					return true;
			}
			throw std::overflow_error("Insertion exceeded max probing distance");
		}

        Value& At(const Key& p_key)
        {
            Hash        hash        = HashFunction(p_key);
            uint64_t    initIndex   = hash % m_bucketsCount;

            for (uint64_t i = 0; i < m_maxProbe; ++i)
            {
                uint64_t currentIndex           = (initIndex + i) % m_bucketsCount;
                uint64_t currentProbeDistance   = DistanceToInitIndex(currentIndex);

                if (m_buckets[currentIndex].data == nullptr || i > currentProbeDistance)
                {
                    throw std::out_of_range("Entry not found");
                }
                if (m_buckets[currentIndex].hash == hash)
                    return *m_buckets[currentIndex].data;
            }
            throw std::overflow_error("Insertion exceeded max probing distance");
        }

        template <typename Function>
        void ForEach(Function p_func)
        {
            uint64_t    initIndex = 0;
            for (uint64_t i = 0; i < m_maxProbe; ++i)
            {
                uint64_t currentIndex = (initIndex + i) % m_bucketsCount;
                uint64_t currentProbeDistance = DistanceToInitIndex(currentIndex);

                if (m_buckets[currentIndex].data != nullptr)
                {
                    p_func(*m_buckets[currentIndex].data);
                }
            }
        }

        Value* Front()
        {
            uint64_t    initIndex = 0;
            for (uint64_t i = 0; i < m_maxProbe; ++i)
            {
                uint64_t currentIndex = (initIndex + i) % m_bucketsCount;
                uint64_t currentProbeDistance = DistanceToInitIndex(currentIndex);

                if (m_buckets[currentIndex].data != nullptr)
                {
                    return m_buckets[currentIndex].data;
                }
                else if (i > currentProbeDistance)
                    return nullptr;
            }
            return nullptr;
        }

    private:

        inline uint64_t DistanceToInitIndex(uint64_t p_index) const
        {
            if (m_buckets[p_index].data == nullptr)
                throw std::invalid_argument("Empty bucket");

            uint64_t initIndex = m_buckets[p_index].hash % m_bucketsCount;
            return (initIndex <= p_index) ? p_index - initIndex : p_index + (m_bucketsCount - initIndex);
        }

        #define get16bits(d) (*((const uint16_t *) (d)))
        using Hash = uint32_t;
        static constexpr Hash HashFunction(const Key& p_key)
        {
            const char * data = reinterpret_cast<const char*>(&p_key);
            int len = sizeof(p_key);
            uint32_t hash = len, tmp;
            int rem;

            if (len <= 0 || data == NULL) return 0;

            rem = len & 3;
            len >>= 2;

            /* Main loop */
            for (; len > 0; len--) {
                hash += get16bits(data);
                tmp = (get16bits(data + 2) << 11) ^ hash;
                hash = (hash << 16) ^ tmp;
                data += 2 * sizeof(uint16_t);
                hash += hash >> 11;
            }

            /* Handle end cases */
            switch (rem) {
            case 3: hash += get16bits(data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof(uint16_t)]) << 18;
                hash += hash >> 11;
                break;
            case 2: hash += get16bits(data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
            case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
            }

            /* Force "avalanching" of final 127 bits */
            hash ^= hash << 3;
            hash += hash >> 5;
            hash ^= hash << 4;
            hash += hash >> 17;
            hash ^= hash << 25;
            hash += hash >> 6;

            return hash;
        }


    private:
        template<typename T>
        struct Bucket
        {
            Hash    hash;
            T*      data;
        };

        Memory::PoolAllocator   m_allocator;
        Bucket<Value>*          m_buckets           {nullptr};
        uint64_t                m_bucketsCount;
        uint64_t                m_bucketsUsed;
        uint64_t                m_maxProbe;
    };
}