#pragma once
#include "ChaosCore/chaos_core_pch.h"

namespace Chaos::Events
{
	class AEvent;
	class IEventDelegate;

	class IEventDispatcher
	{
	public:
		virtual ~IEventDispatcher() = default;

		virtual void Dispatch(AEvent& p_event) = 0;
		
		virtual void AddEventCallback(IEventDelegate&) = 0;

		virtual void RemoveEventCallback(IEventDelegate&) = 0;


	private:

	};
}