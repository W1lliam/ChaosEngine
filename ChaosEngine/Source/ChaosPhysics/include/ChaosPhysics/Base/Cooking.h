#pragma once
#include "ChaosPhysics/Base/PhysicsFoundation.h"
namespace Chaos
{
	namespace Physics
	{
		class Cooking
		{
		public:
			Cooking() = default;
			~Cooking() = default;
			void InitCooking(Foundation p_foundation, physx::PxCookingParams p_cookingParam);
			void DestroyCooking() const;
			physx::PxCooking* GetCooking() const;
		private:
			physx::PxCooking* m_cooking = nullptr;
		};
	}
}