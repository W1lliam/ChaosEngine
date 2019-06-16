#pragma once

#include "IEventDelegate.h"

namespace Chaos::Events
{
	template<class T>
	class EventDelegate : public IEventDelegate
	{
	public:
        EventDelegate(const std::function<bool(const T&)>& p_func) : IEventDelegate{ T::GetStaticType(), s_delegateCount++ }, m_callback{ p_func } { }

		EventDelegate() = default;

		inline bool operator()(const AEvent& p_event)const override
		{
			return m_callback(reinterpret_cast<const T&>(p_event));
		}

		inline bool operator==(const IEventDelegate& p_other)const override
		{
			if (m_type != p_other.GetType())
				return false;

			const EventDelegate<T>& delegate = reinterpret_cast<const EventDelegate<T>&>(p_other);

			return m_id == delegate.m_id;
		}

	private:
		std::function<bool(const T&)> m_callback;
        inline static ObjectID s_delegateCount = 0;

	};
}