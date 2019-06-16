#pragma once
#include "ChaosCore/chaos_core_pch.h"

namespace Chaos::Containers
{
	template<typename T>
	class ThreadSafeList
	{
	private:
		struct Node;

	public:
		ThreadSafeList() = default;
		ThreadSafeList(const ThreadSafeList&) = delete;
		ThreadSafeList& operator=(const ThreadSafeList&) = delete;

		~ThreadSafeList()
		{
            clear();
		}

        inline uint32_t size() const { return m_size; }

        void push_front(const T& p_value)
        {
            std::unique_ptr<Node> newNode(new Node(p_value));
            std::lock_guard<std::shared_mutex> lock(m_head.mutex);
            newNode->next = std::move(m_head.next);
            m_head.next = std::move(newNode);
            m_size++;
        }

        void push_back(const T& p_value)
        {
            Node* current = &m_head;
            {
                std::shared_lock<std::shared_mutex> lock(m_head.mutex);
                while (Node* const next = current->next.get())
                {
                    std::shared_lock<std::shared_mutex> nextLock(next->mutex);
                    lock.unlock();
                    current = next;
                    lock = std::move(nextLock);
                }
            }
            std::unique_lock<std::shared_mutex> lock(current->mutex);
            std::unique_ptr<Node> newNode(new Node(p_value));
            newNode->next = std::move(current->next);
            current->next = std::move(newNode);
            m_size++;
        }

        std::shared_ptr<T>  front()
		{
			std::lock_guard<std::shared_mutex> lock(m_head.mutex);
            return m_head.next->data;
		}


		template <typename Function>
		void for_each(Function p_func)
		{
			Node* current = &m_head;
			std::unique_lock<std::shared_mutex> lock(m_head.mutex);
			while(Node* const next = current->next.get())
			{
				std::unique_lock<std::shared_mutex> nextLock(next->mutex);
				lock.unlock();
				p_func(*next->data);
				current = next;
				lock = std::move(nextLock);
			}
		}

		template<typename Predicate>
		std::shared_ptr<T> find_if(Predicate p_predicate)
		{
			Node* current = &m_head;
			std::unique_lock<std::shared_mutex> lock(m_head.mutex);
			while(Node* const next = current->next.get())
			{
				std::unique_lock<std::shared_mutex> nextLock(next->mutex);
				lock.unlock();
				if(p_predicate(*next->data))
				{
					return next->data;
				}
				current = next;
				lock = std::move(nextLock);
			}

			return std::shared_ptr<T>();
		}

        template<typename Predicate>
        void remove_if(Predicate p_predicate)
        {
            Node* current = &m_head;
            std::unique_lock<std::shared_mutex> lock(m_head.mutex);
            while (Node* const next = current->next.get())
            {
                std::unique_lock<std::shared_mutex> nextLock(next->mutex);
                if (p_predicate(*next->data))
                {
                    std::unique_ptr<Node> oldNext = std::move(current->next);
                    current->next = std::move(next->next);
                    nextLock.unlock();
                    --m_size;
                }
                else
                {
                    lock.unlock();
                    current = next;
                    lock = std::move(nextLock);
                }
            }
        }

        void clear()
        {
            Node* current = &m_head;
            std::unique_lock<std::shared_mutex> lock(m_head.mutex);
            while (Node* const next = current->next.get())
            {
                std::unique_lock<std::shared_mutex> nextLock(next->mutex);
                std::unique_ptr<Node> oldNext = std::move(current->next);
                current->next = std::move(next->next);
                nextLock.unlock();
                --m_size;
            }
        }

		private:
			struct Node
			{
				std::shared_mutex mutex;
				std::shared_ptr<T> data;
				std::unique_ptr<Node> next;

				Node() : next() {};
				Node(const T& p_value) : data(std::make_shared<T>(p_value)){};
			};

			Node        m_head;
            uint32_t    m_size  {0};
	};
}
