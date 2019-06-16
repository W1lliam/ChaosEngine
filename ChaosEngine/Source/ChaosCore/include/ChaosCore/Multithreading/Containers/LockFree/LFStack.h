#pragma once
#include "ChaosCore/chaos_core_pch.h"

namespace Chaos::Containers::LockFree
{
	template <typename T>
	class LFStack
	{
	private:
        struct Node;
        struct CountedNodePtr;

	public:
		LFStack()
		{};

		~LFStack()
		{
			while(pop());
		}

		void push(T const& p_data)
		{
			CountedNodePtr newNode{};
			newNode.ptr = new Node(p_data);
			newNode.externalCount = 1;
			newNode.ptr->next = m_head.load(std::memory_order_relaxed);
			while(!m_head.compare_exchange_weak(newNode.ptr->next, newNode, std::memory_order_release, std::memory_order_relaxed));
		}

		std::shared_ptr<T> pop()
		{
			CountedNodePtr oldHead = m_head.load(std::memory_order_relaxed);
			while(true)
			{
				IncreaseHeadCount(oldHead);
				Node* const ptr = oldHead.ptr;
				if(!ptr)
					return std::shared_ptr<T>();
				if(m_head.compare_exchange_strong(oldHead,ptr->next, std::memory_order_relaxed, std::memory_order_relaxed))
				{
					std::shared_ptr<T> res;
					res.swap(ptr->data);

					const int countIncrease = oldHead.externalCount - 2;

					if(ptr->internalCount.fetch_add(countIncrease, std::memory_order_release) == countIncrease)
					{
						delete ptr;
					}
					return res;
				}
				else if(ptr->internalCount.fetch_sub(1, std::memory_order_relaxed) == 1)
				{
					ptr->internalCount.load(std::memory_order_acquire);
					delete ptr;
				}
			}
		}

    private:
        void IncreaseHeadCount(CountedNodePtr& p_oldCounter)
        {
            CountedNodePtr newCounter;
            do
            {
                newCounter = p_oldCounter;
                ++newCounter.externalCount;
            } while (!m_head.compare_exchange_strong(p_oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));
            p_oldCounter.externalCount = newCounter.externalCount;
        }

        struct CountedNodePtr
        {
            int externalCount{ 0 };
            Node* ptr{ nullptr };

            CountedNodePtr() = default;
            CountedNodePtr(int p_extCount, Node* p_ptr) : externalCount(p_extCount), ptr(p_ptr) {}
        };

        struct Node
        {
            std::shared_ptr<T>	data;
            std::atomic<int>	internalCount{ 0 };
            CountedNodePtr		next;

            Node(T const& p_data) : data(std::make_shared<T>(p_data)), next(0, nullptr)
            {}
        };

        std::atomic<CountedNodePtr> m_head;
	};
}
