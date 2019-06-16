#pragma once
#include "PxPhysicsAPI.h"
#include <Vector>
#include <string>
namespace Chaos
{
	namespace Physics
	{
		class CollisionFilter
		{
		public:
			CollisionFilter();
			void UpdateFilter(int p_index, bool p_block);
			physx::PxFilterData GetFilterData();
		private:
			std::vector<bool> m_filterValues;
		};
	}
}