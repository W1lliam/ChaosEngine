#include "ChaosCore/Events/EventHandler.h"

namespace Chaos::Events
{
	EventHandler::EventHandler() : m_dispatcherMap((uint64_t)E_EventType::EVENT_COUNT), m_memory{ 81920 }, m_memoryDispatcher{sizeof(EventDispatcher<AEvent>) * (uint64_t)E_EventType::EVENT_COUNT }
	{
        CHAOS_CORE_TRACE("Initializing EventHandler");

        m_memory.Init();
        m_memoryDispatcher.Init();
	}

	EventHandler::~EventHandler()
	{
        CHAOS_CORE_TRACE("Shutting down EventHandler");
        m_dispatcherMap.Close();
        m_eventData.clear();
        m_memory.Clear();
        m_memoryDispatcher.Clear();
	}
    
    void EventHandler::DispatchEvents()
    {
        size_t lastIndex = this->m_eventData.size();
        size_t thisIndex = 0;

        while (thisIndex < lastIndex)
        {
            auto event = m_eventData[thisIndex++];
            if (event == nullptr)
            {
                CHAOS_CORE_WARN("Skip corrupted event. {0}", event->GetName());
                continue;
            }

            if (!m_dispatcherMap.Contains(event->GetType()))
                continue;

            m_dispatcherMap.At(event->GetType())->Dispatch(*event);

            lastIndex = this->m_eventData.size();
        }

        ClearEventBuffer();
    }
}