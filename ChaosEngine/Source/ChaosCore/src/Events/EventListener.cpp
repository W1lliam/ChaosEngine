#include "ChaosCore/Events/EventListener.h"

namespace Chaos::Events
{
	EventListener::EventListener() : m_memory{(size_t)E_EventType::EVENT_COUNT * sizeof(EventDelegate<AEvent>), sizeof(EventDelegate<AEvent>), alignof(EventDelegate<AEvent>)}
	{
		
	}
	EventListener::~EventListener() 
    {
        UnregisterAllEventCallbacks();
	}

	void EventListener::UnregisterAllEventCallbacks()
	{
		for (auto& callback : m_registeredCallback)
		{
			EventHandler::Get().RemoveEventCallback(*callback);
		}

		m_registeredCallback.clear();
		m_memory.Clear();
	}
}