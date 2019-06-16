#pragma once
#pragma warning(push)
#pragma warning(disable: 4251)
#include "ChaosCore/chaos_core_pch.h"

namespace Chaos::Containers::LockFree
{
	template<typename T>
	class LFQueue
	{
				
	#ifdef _M_X64 
		using AlignedInt = int64_t;
	#else
		using AlignedInt = int32_t;
	#endif

	private:
        struct Node;
        struct CountedNodePtr;
        struct NodeCounter;

	public:
		LFQueue() : m_head(CountedNodePtr{new Node, 1}), m_tail(m_head.load())
		{}
		LFQueue(const LFQueue& other) = delete;
		LFQueue& operator=(const LFQueue& other) = delete;

		~LFQueue()
		{
			while(pop());

            delete m_tail.load().ptr;
            m_tail.store({nullptr, 0});
                    

		}

		void push(T p_value)
		{
			T* newData = new T(std::move(p_value));
			CountedNodePtr newNext;
			newNext.ptr = new Node;
			newNext.externalCount = 1;
			CountedNodePtr oldTail = m_tail.load();
			while (true)
			{
				IncreaseExternalCount(m_tail, oldTail);
				T* oldData = nullptr;
				if(oldTail.ptr->data.compare_exchange_strong(oldData,newData))
				{
					CountedNodePtr oldNext;

					if(!oldTail.ptr->next.compare_exchange_strong(oldNext, newNext))
					{
						delete newNext.ptr;
						newNext = oldNext;
					}

					SetNewTail(oldTail,	newNext);
					break;
				}
				else
				{
					CountedNodePtr oldNext;
					if(oldTail.ptr->next.compare_exchange_strong(oldNext, newNext))
					{
						oldNext = newNext;
						newNext.ptr = new Node;
					}
					SetNewTail(oldTail,	oldNext);
				}
			}
		}

		std::unique_ptr<T> pop()
		{
			CountedNodePtr oldHead = m_head.load(std::memory_order_relaxed);

			while (true)
			{
				IncreaseExternalCount(m_head, oldHead);
				Node* ptr = oldHead.ptr;
				if(ptr == m_tail.load().ptr)
				{
					return std::unique_ptr<T>();
				}
				CountedNodePtr next = ptr->next.load();
				if(m_head.compare_exchange_strong(oldHead, next))
				{
					T* const res = ptr->data.exchange(nullptr);
                    delete oldHead.ptr;

					return std::unique_ptr<T>(res);
				}
				ptr->ReleaseRef();
			}
		}

        private:
        static void IncreaseExternalCount(std::atomic<CountedNodePtr>& p_counter, CountedNodePtr& p_oldCounter)
        {
            CountedNodePtr newCounter;
            do
            {
                newCounter = p_oldCounter;
                ++newCounter.externalCount;
            } while (!p_counter.compare_exchange_strong(p_oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));
            p_oldCounter.externalCount = newCounter.externalCount;
        }

        static void FreeExternalCounter(CountedNodePtr& p_oldNodePtr)
        {
            Node* ptr = p_oldNodePtr.ptr;
            const AlignedInt countIncrease = p_oldNodePtr.externalCount - 2;

            NodeCounter oldCounter = ptr->counter.load(std::memory_order_relaxed);
            NodeCounter newCounter{};

            do
            {
                newCounter = oldCounter;
                --newCounter.externalCounters;
                newCounter.internalCount += static_cast<int>(countIncrease);
            } while (!ptr->counter.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));

            if (newCounter.internalCount == 0 && newCounter.externalCounters == 0)
            {
                delete ptr;
                ptr = nullptr;
            }
        }

        void SetNewTail(CountedNodePtr& p_oldTail, const CountedNodePtr& p_newTail)
        {
            Node* const currentTailPtr = p_oldTail.ptr;
            while (!m_tail.compare_exchange_weak(p_oldTail, p_newTail) && p_oldTail.ptr == currentTailPtr);


            if (p_oldTail.ptr == currentTailPtr)
                FreeExternalCounter(p_oldTail);
            else
                currentTailPtr->ReleaseRef();
        }

        std::atomic<CountedNodePtr> m_head;
        std::atomic<CountedNodePtr> m_tail;

        struct CountedNodePtr
        {
            Node*		ptr{ nullptr };
            AlignedInt	externalCount{ 0 };
        };

        struct NodeCounter
        {
            unsigned internalCount : 30;
            unsigned externalCounters : 2;
        };

        struct Node
        {
            std::atomic<T*>				data{ nullptr };
            std::atomic<NodeCounter>	counter;
            std::atomic<CountedNodePtr>	next;

            Node()
            {
                NodeCounter newCounter{};
                newCounter.internalCount = 0;
                newCounter.externalCounters = 2;
                counter.store(newCounter);
            }

            void ReleaseRef()
            {
                NodeCounter oldCounter = counter.load(std::memory_order_relaxed);
                NodeCounter newCounter{};
                do
                {
                    newCounter = oldCounter;
                    if (newCounter.internalCount > 0)
                        --newCounter.internalCount;
                } while (!counter.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));

                if (newCounter.internalCount == 0 && newCounter.externalCounters == 0)
                {
                    delete data.load();
                    delete this;
                }
            }
        };
	};
}

#pragma warning(pop)