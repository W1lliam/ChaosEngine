#pragma once

#include "ChaosCore/Events/AEvent.h"
#include "KeyCodes.h"

namespace Chaos::Events
{
	class AKeyEvent : public AEvent
	{
	public:
		inline E_ChaosKeyCode KeyCode() const { return m_keyCode; }

		EVENT_CLASS_CATEGORY(E_EVENTCATEGORY_KEYBOARD | E_EVENTCATEGORY_INPUT)

	protected:

		AKeyEvent(E_EventType eType, E_ChaosKeyCode p_keycode) : AEvent(eType), m_keyCode(p_keycode) {}

		E_ChaosKeyCode m_keyCode;

	};

	class KeyPressedEvent : public AKeyEvent
	{
	public:

		KeyPressedEvent(E_ChaosKeyCode p_keycode, uint32_t p_repeatCount) : AKeyEvent(E_EventType::KEY_PRESSED, p_keycode), m_repeatCount(p_repeatCount) {}

		inline uint32_t GetRepeatCount() const { return m_repeatCount; }

		inline void Print() const override
		{
			CHAOS_CORE_INFO("KeyPressedEvent:\n\tKey({0} ({1} repeats))", (char*)&m_keyCode, m_repeatCount);
		}

		EVENT_CLASS_TYPE(KEY_PRESSED)

	private:

		uint32_t m_repeatCount;
	};

	class KeyReleasedEvent : public AKeyEvent
	{
	public:
		KeyReleasedEvent(E_ChaosKeyCode p_keyCode) : AKeyEvent(E_EventType::KEY_RELEASED, p_keyCode) {}

		inline void Print() const override
		{
			CHAOS_CORE_INFO("KeyReleasedEvent:\n\tKey({0})", (char*)&m_keyCode);
		}

		EVENT_CLASS_TYPE(KEY_RELEASED)
	};
}