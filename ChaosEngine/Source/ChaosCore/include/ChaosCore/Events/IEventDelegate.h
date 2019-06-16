#pragma once

#include "ChaosCore/Events/AEvent.h"
#include "ChaosCore/Utils/GUID.h"

namespace Chaos::Events
{
	class AEvent;
	class IEventDelegate
	{
	public:
		IEventDelegate(E_EventType p_type, ObjectID p_id) : m_type{ p_type }, m_id{p_id} { }

		virtual bool operator()(const AEvent&)const = 0;

		virtual bool operator==(const IEventDelegate& p_other) const = 0;

		inline constexpr E_EventType GetType() const { return m_type; }

		inline constexpr ObjectID GetID() const { return m_id; }

	protected:
		E_EventType m_type;
        ObjectID m_id;
	};
}