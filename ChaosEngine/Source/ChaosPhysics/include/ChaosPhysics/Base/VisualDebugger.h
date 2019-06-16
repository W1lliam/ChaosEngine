#pragma once
#include "ChaosPhysics/Base/PhysicsFoundation.h"

namespace Chaos
{
	namespace Physics
	{
		class VisualDebugger
		{
		public:
			VisualDebugger() = default;
			~VisualDebugger() = default;
			void InitVisualDebugger(Foundation p_foundation);
			physx::PxPvd* GetDebugger() const;
		private:
			physx::PxPvd* m_visualDebugger = nullptr;
		};
	}
}