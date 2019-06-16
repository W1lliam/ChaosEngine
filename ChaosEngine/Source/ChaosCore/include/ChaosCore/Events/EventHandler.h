#pragma once

#include"ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Events/EventDispatcher.h"
#include "ChaosCore/Utils/Singleton.h"
#include "ChaosCore/Containers/RHHashTable.h"
#include "ChaosCore/Memory/Allocators/LinearAllocator.h"

namespace Chaos::Events
{
	class EventHandler : public Singleton<EventHandler>
	{
	public:
        EventHandler();
        ~EventHandler();

		EventHandler(const EventHandler&) = delete;
		EventHandler& operator=(EventHandler&) = delete;

        template<class T>
		inline void AddEventCallback(IEventDelegate& p_delegate)
		{
			E_EventType eventID = p_delegate.GetType();

			if (!m_dispatcherMap.Contains(eventID))
			{
				void* mem = m_memoryDispatcher.Allocate(sizeof(EventDispatcher<T>), alignof(EventDispatcher<T>));
				auto newDispatcher = reinterpret_cast<IEventDispatcher*>(new (mem)EventDispatcher<T>());
				m_dispatcherMap.Insert(eventID, newDispatcher);
				m_dispatcherMap.At(eventID)->AddEventCallback(p_delegate);
			}
			else
			{
				m_dispatcherMap.At(eventID)->AddEventCallback(p_delegate);
			}

		}

		inline void RemoveEventCallback(IEventDelegate& p_delegate)
		{
			E_EventType eventID = p_delegate.GetType();
			if (m_dispatcherMap.Contains(eventID))
			{
				m_dispatcherMap.At(eventID)->RemoveEventCallback(p_delegate);
			}
		}

		inline void ClearEventBuffer()
		{
			m_memory.Clear();
			m_eventData.clear();
		}

		inline void CleanEventDispatcher()
		{
			m_dispatcherMap.Close();
			m_memoryDispatcher.Clear();
		}

		template<class T, class... ARGS>
		inline void Send(ARGS&&... p_eventArgs)
		{
			void* mem = m_memory.Allocate(sizeof(T), alignof(T));
			if (mem != nullptr)
			{
				m_eventData.push_back(new (mem)T(std::forward<ARGS>(p_eventArgs)...));
			}
			else
			{
				CHAOS_CORE_WARN("Event buffer fulll");
			}
		}

        void DispatchEvents();
    
    private:
		// Not Multithreaded
		RHHashTable<E_EventType, IEventDispatcher*> 					m_dispatcherMap;
		std::vector<AEvent*>											m_eventData;
		Memory::LinearAllocator											m_memory;
		Memory::LinearAllocator											m_memoryDispatcher;
	};
}
