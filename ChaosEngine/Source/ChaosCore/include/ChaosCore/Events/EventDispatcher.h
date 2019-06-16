#pragma once

#include "AEvent.h"
#include "ChaosCore/Multithreading/Containers/ThreadSafeList.h"
#include "ChaosCore/Events/IEventDispatcher.h"
#include "ChaosCore/Events/EventDelegate.h"

namespace Chaos::Events
{
	template<class T>
	class EventDispatcher : public IEventDispatcher
	{
		using CallBack = IEventDelegate;

		static_assert(std::is_base_of<AEvent, T>::value, "ERROR: Invalid type");
	public:
		EventDispatcher() = default;

		inline ~EventDispatcher() override
		{
			this->m_eventCallbacks.clear();
		}

		inline void AddEventCallback(CallBack& p_callback) override
		{
			m_eventCallbacks.push_back(reinterpret_cast<EventDelegate<T>&>(p_callback));
		}

		void Dispatch(AEvent& p_event) override
		{
			for (auto& callback : m_eventCallbacks)
			{
				if(!p_event.IsHandled())
					p_event.Handle(callback(p_event));
			}
		}

		inline void RemoveEventCallback(CallBack& p_callback) override
		{
			m_eventCallbacks.remove_if([&p_callback](CallBack& callback) { return p_callback == callback; });
		}

		inline virtual uint32_t GetEventCallbackCount() const { return static_cast<uint32_t>(m_eventCallbacks.size()); }

	private:
		std::list<EventDelegate<T>> m_eventCallbacks{};
	};
}