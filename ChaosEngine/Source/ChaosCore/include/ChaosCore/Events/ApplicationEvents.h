#pragma once

#include "ChaosCore/Events/AEvent.h"

namespace Chaos::Events
{
	class WindowResizeEvent : public AEvent
	{
	public:
		WindowResizeEvent(uint32_t p_width, uint32_t p_height) : AEvent(E_EventType::WINDOW_RESIZED), m_width(p_width), m_height(p_height) {}

		inline uint32_t GetWidth()	const { return m_width; }
		inline uint32_t GetHeight() const { return m_height; }

		inline void Print() const override
		{
			CHAOS_CORE_INFO("WindowResizeEvent:\n\tSize: {0}x{1}", m_width, m_height);
		}

		EVENT_CLASS_TYPE(WINDOW_RESIZED)
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_APPLICATION)

	private:
		uint32_t m_width, m_height;

	};

	class WindowsCloseEvent : public AEvent
	{
	public:
		WindowsCloseEvent() : AEvent(E_EventType::WINDOW_CLOSED) {}

		EVENT_CLASS_TYPE(WINDOW_CLOSED)
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_APPLICATION)

	};

	class TickEvent : public AEvent
	{
	public:
		TickEvent(): AEvent(E_EventType::TICKED) {}

		EVENT_CLASS_TYPE(TICKED)
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_APPLICATION)

	};

	class UpdateEvent : public AEvent
	{
	public:
		UpdateEvent() : AEvent(E_EventType::UPDATED) {}

		EVENT_CLASS_TYPE(UPDATED)
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_APPLICATION)

	};

	class RenderEvent : public AEvent
	{
	public:
		RenderEvent() : AEvent(E_EventType::RENDERED) {}

		EVENT_CLASS_TYPE(RENDERED)
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_APPLICATION)
	};
}