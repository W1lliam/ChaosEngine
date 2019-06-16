#pragma once
#pragma warning (push)
#pragma warning (disable:4251)

#include "ChaosCore/chaos_core_pch.h"
#include "IEventDelegate.h"
#include "EventHandler.h"
#include "ChaosCore/Memory/Allocators/PoolAllocator.h"

namespace Chaos::Events
{
	class EventListener
	{
	public:
		EventListener();

		virtual ~EventListener();

		template<class T>
		inline void RegisterEventCallback(const std::function<bool(const T&)>& p_callback)
		{
			void* mem = m_memory.Allocate(sizeof(EventDelegate<AEvent>), alignof(EventDelegate<AEvent>));
            IEventDelegate* delegate = new (mem)EventDelegate<T>(p_callback);
			m_registeredCallback.push_back(delegate);
			EventHandler::Get().AddEventCallback<T>(*delegate);
		}

		template<class T>
		inline void UnregisterEventCallback(std::function<bool(const T&)> p_callback)
		{
			for (auto delegate : m_registeredCallback)
			{
				if(delegate == p_callback)
				{
					EventHandler::Get().RemoveEventCallback(p_callback);
					m_registeredCallback.remove(delegate);
					m_memory.Free(delegate);
				}
			}
		}

		void UnregisterAllEventCallbacks();

	private:
		std::list<IEventDelegate*> m_registeredCallback;
		Memory::PoolAllocator m_memory;
	};
}
#pragma warning (pop)