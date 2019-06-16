#pragma once

#include "ChaosCore/chaos_core_pch.h"
#include "ChaosDebug.h"

#ifdef MOUSE_MOVED
	#undef	MOUSE_MOVED
#endif


namespace Chaos::Events
{

#define EVENT_CLASS_TYPE(type) static E_EventType GetStaticType() { return E_EventType::##type; }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual uint8_t GetCategoryFlag() const override { return category; }

	// WARNING: do not exceed the bit count limit, in due case modify the enumtype.
	enum class E_EventType : uint8_t
	{

		NONE = 0,
		WINDOW_CLOSED,
		WINDOW_RESIZED,
		WINDOW_FOCUSED,
		WINDOW_LOST_FOCUS,
		WINDOW_MOVED,
		TICKED,
		UPDATED,
		RENDERED,
		KEY_PRESSED,
		KEY_RELEASED,
		MOUSE_BUTTON_PRESSED,
		MOUSE_BUTTON_RELEASED,
		MOUSE_MOVED,
        MOUSE_SCROLLED,
        ENTITY_CREATED,
        ENTITY_REMOVED,
		SCENE_CHANGED,
		RIGIDBODY_CREATED,
		RIGIDBODY_REMOVED,
		BOXCOLLIDER_REMOVED,
		SPHERECOLLIDER_REMOVED,
		COMPONENT_CREATED,
		COMPONENT_ABOUT_TO_BE_REMOVED,
		COMPONENT_REMOVED,
		ENTITY_RENAMED,
		//TODO: Add Joystick type.

		EVENT_COUNT
	};

	// WARNING: do not exceed the bit count limit, in due case modify the enumtype.
	enum EventCategory : uint8_t
	{
		E_EVENTCATEGORY_NONE		= 0,
		E_EVENTCATEGORY_APPLICATION = 1 << 0,
		E_EVENTCATEGORY_INPUT		= 1 << 1,
		E_EVENTCATEGORY_KEYBOARD	= 1 << 2,
		E_EVENTCATEGORY_MOUSE		= 1 << 3,
        E_EVENTCATEGORY_MOUSEBUTTON = 1 << 4,
        E_EVENTCATEGORY_SCENE       = 1 << 5,
        E_EVENTCATEGORY_ECS         = 1 << 6
	};

	class AEvent
	{
	public:
		AEvent(E_EventType p_type) : m_type(p_type) {}
		virtual E_EventType GetType()		const { return m_type; };
		virtual const char* GetName()		const = 0;

		// WARNING: make sure the following function's return type is the same or grater than EventCategory type
		virtual uint8_t GetCategoryFlag()	const = 0;

		virtual void	Print()				const
		{
			CHAOS_CORE_INFO("{0}", GetName());
		}

		inline bool IsInCategory(EventCategory p_category)
		{
			return GetCategoryFlag() & p_category;
		}

		inline bool IsHandled()
		{
			return m_handled;
		}
		inline void Handle(bool p_handle)
		{
			m_handled = p_handle;
		}

	protected:
		bool m_handled = false;
		E_EventType m_type;

	};
}