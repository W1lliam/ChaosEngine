#pragma once
#include "ChaosCore/chaos_core_pch.h"

#include <ChaosDebug.h>

namespace Chaos::Containers
{
	template<typename Key, typename Value, typename Hash = std::hash<Key>>
	class ThreadSafeMap
	{

    private:
        class BucketType;

	public:
		using BucketValue			= std::pair<Key,Value>;
		using BucketData			= std::list<BucketValue>;
		using BucketIterator		= typename std::list<BucketValue>::iterator;
		using BucketConstIterator	= typename std::list<BucketValue>::const_iterator;

		using key_type      = Key;
		using mapped_type   = Value;
		using hash_type     = Hash;

		ThreadSafeMap(unsigned p_numBuckets = 19, Hash const& p_hashFunc = Hash()): m_buckets(p_numBuckets), m_hashFunction(p_hashFunc)
		{
			for (unsigned i = 0; i < p_numBuckets; ++i)
				m_buckets[i].reset(new BucketType);
		}

        ThreadSafeMap(const ThreadSafeMap<Key, Value, Hash>& p_other) : 
            m_buckets(p_other.m_buckets.size()), 
            m_hashFunction(p_other.m_hashFunction),
            m_size(p_other.m_size)
        {
            for (uint32_t i = 0; i < m_buckets.size(); ++i)
            {
                m_buckets[i].reset(new BucketType);
                std::unique_lock<std::shared_mutex> lock(m_buckets[i]->m_mutex);
                std::shared_lock<std::shared_mutex> otherLock(p_other.m_buckets[i]->m_mutex);
                m_buckets[i]->m_data = p_other.m_buckets[i]->m_data;
            }

        }

		ThreadSafeMap& operator=(const ThreadSafeMap&) = delete;

        ~ThreadSafeMap()
		{
            for (uint32_t i = 0; i < m_buckets.size(); ++i)
            {
                m_buckets[i]->clear();
                CHAOS_CORE_ASSERT(m_buckets[i]->m_data.empty(), "Failed to clear bucket!");
                m_buckets[i].reset();
            }
		}

        Value at(const Key& p_key)
        {
            return GetBucket(p_key).at(p_key);
        }

        BucketValue front()
        {
            for (unsigned i = 0; i < m_buckets.size(); ++i)
            {
                if (!m_buckets[i]->m_data.empty())
                    return *m_buckets[i]->m_data.begin();
            }
            return {};
        }
        BucketValue back()
        {
            for (unsigned i = 0; i < m_buckets.size(); ++i)
            {
                if (!m_buckets[i]->m_data.empty())
                    return *(++m_buckets[i]->m_data.rend());
            }
            return {};
        }

		bool exists(const Key& p_key)
		{
;			return GetBucket(p_key).exists(p_key);
		}

		void insert_or_update(const Key& p_key, const Value& p_value)
		{
            if (GetBucket(p_key).insert_or_update(p_key, p_value))
                ++m_size;
		}

		void erase(const Key& p_key)
		{
            if (GetBucket(p_key).erase(p_key))
                --m_size;
		}

		void clear()
		{
            for (unsigned i = 0; i < m_buckets.size(); ++i)
            {
                if (!m_buckets[i]->m_data.empty())
                    m_buckets[i]->clear();
            }
            m_size = 0;
		}

        uint32_t size()
        {
            return m_size;
        }

        template <typename Function>
        void for_each(Function p_func)
        {
            for (unsigned i = 0; i < m_buckets.size(); ++i)
                m_buckets[i]->for_each(p_func);
        }

		std::map<Key,Value> ToSTLMap() const
		{
			std::vector<std::unique_lock<std::shared_mutex>> locks;
			for(unsigned i = 0; i < m_buckets.size(); ++i)
			{
				locks.emplace_back(m_buckets[i]->m_mutex);
			}

			std::map<Key,Value> res;
			for(unsigned i = 0; i < m_buckets.size(); ++i)
			{
				for(BucketIterator it=m_buckets[i]->m_data.begin(); it!=m_buckets[i]->m_data.end(); ++it)
					res.insert(*it);
			}
			return res;
		}

    private:
        BucketType& GetBucket(const Key& p_key) const
        {
            std::size_t const bucketIndex = m_hashFunction(p_key) % m_buckets.size();
            return *m_buckets[bucketIndex];
        }

        class BucketType
        {
            friend ThreadSafeMap;
        public:
            Value at(const Key& p_key)
            {
                std::shared_lock<std::shared_mutex> lock(m_mutex);
                BucketConstIterator foundEntry = FindEntry(p_key);
                CHAOS_CORE_ASSERT(foundEntry != m_data.end(), "Check with exists() before calling at() if unsure of the elements' presence");
                return foundEntry->second;
            }
            bool exists(const Key& p_key)
            {
                std::shared_lock<std::shared_mutex> lock(m_mutex);
                BucketConstIterator foundEntry = FindEntry(p_key);
                return (foundEntry != m_data.end());
            }

            bool insert_or_update(const Key& p_key, const Value& p_value)
            {
                std::unique_lock<std::shared_mutex> lock(m_mutex);
                const BucketIterator foundEntry = FindEntry(p_key);
                if (foundEntry == m_data.end())
                {
                    m_data.push_back(BucketValue(p_key, p_value));
                    return true;
                }
                else
                {
                    foundEntry->second = p_value;
                    return false;
                }
            }

            bool erase(const Key& p_key)
            {
                std::unique_lock<std::shared_mutex> lock(m_mutex);
                BucketConstIterator foundEntry = FindEntry(p_key);
                if (foundEntry != m_data.end())
                {
                    m_data.erase(foundEntry);
                    return true;
                }
                return false;
            }

            void clear()
            {
                std::unique_lock<std::shared_mutex> lock(m_mutex);
                m_data.clear();
            }

            template <typename Function>
            void for_each(Function p_func)
            {
                std::unique_lock<std::shared_mutex> lock(m_mutex);
                for (BucketValue& valuePair : m_data)
                    p_func(valuePair);
            }

        private:
            BucketConstIterator FindEntry(const Key& p_key) const
            {
                return std::find_if(m_data.begin(), m_data.end(),
                    [&](const BucketValue& pair) {return pair.first == p_key; });
            }
            BucketIterator FindEntry(const Key& p_key)
            {
                return std::find_if(m_data.begin(), m_data.end(),
                    [&](const BucketValue& pair) {return pair.first == p_key; });
            }

            BucketData m_data{};
            std::shared_mutex m_mutex;
        };

        std::vector<std::unique_ptr<BucketType>> m_buckets;
        Hash m_hashFunction;
        uint32_t    m_size{0};
		
	};


}
