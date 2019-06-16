#pragma once

#include "ChaosCore/Events/AEvent.h"
#include "KeyCodes.h"

namespace Chaos::Events
{
	class MouseMovedEvent : public AEvent
	{
	public:
		MouseMovedEvent(float p_x, float p_y) : AEvent(E_EventType::MOUSE_MOVED),m_mouseX(p_x), m_mouseY(p_y) {}

		inline float GetX() const { return m_mouseX; }
		inline float GetY() const { return m_mouseY; }

		inline void Print() const override
		{
			CHAOS_CORE_INFO("MouseMovedEvent:\n\tPosition{0}, {1}", m_mouseX, m_mouseY);
		}

		EVENT_CLASS_TYPE(MOUSE_MOVED)
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_MOUSE | E_EVENTCATEGORY_INPUT)

	private:
		float m_mouseX, m_mouseY;

	};

	class MouseScrolledEvent : public AEvent
	{
	public:
		MouseScrolledEvent(float p_offsetX, float p_offsetY) : AEvent(E_EventType::MOUSE_SCROLLED), m_offsetX(p_offsetX), m_offsetY(p_offsetY) {}

		inline float GetXOffset() const { return m_offsetX; }
		inline float GetYOffset() const { return m_offsetY; }

		inline void Print() const override
		{
			CHAOS_CORE_INFO("MouseScrolledEvent:\n\tOffsetX({0}), OffsetY({1})", m_offsetX, m_offsetY);
		}

		EVENT_CLASS_TYPE(MOUSE_SCROLLED)
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_MOUSE | E_EVENTCATEGORY_INPUT)

	private:
		float m_offsetX, m_offsetY;

	};

	class MouseButtonEvent : public AEvent
	{
	public:
		inline E_ChaosMouseCode GetMouseButton() const { return m_button; }
		
		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_MOUSE | E_EVENTCATEGORY_MOUSEBUTTON | E_EVENTCATEGORY_INPUT)

	protected:
		MouseButtonEvent(E_EventType eType, E_ChaosMouseCode p_button) :AEvent(eType), m_button(p_button) {}

		E_ChaosMouseCode m_button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(E_ChaosMouseCode p_mouseCode) : MouseButtonEvent(E_EventType::MOUSE_BUTTON_PRESSED, p_mouseCode) {}

		inline void Print() const override
		{
			CHAOS_CORE_INFO("MouseButtonReleasedEvent:\n\tmouseButton{0}", (int8_t)m_button);
		}

		EVENT_CLASS_TYPE(MOUSE_BUTTON_PRESSED)

	private:

	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(E_ChaosMouseCode p_button) : MouseButtonEvent(E_EventType::MOUSE_BUTTON_RELEASED, p_button) {}

		inline void Print() const override
		{
			CHAOS_CORE_INFO("MouseButtonReleasedEvent:\n\tmouseButton{0}", (int8_t)m_button);
		}

		EVENT_CLASS_TYPE(MOUSE_BUTTON_RELEASED)

	private:

	};
}