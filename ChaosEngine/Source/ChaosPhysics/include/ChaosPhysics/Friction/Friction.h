#pragma once
#include "ChaosPhysics/Base/PhysicsElementCreator.h"

namespace Chaos
{
	namespace Physics
	{
		class Friction
		{
		public:
			Friction(float p_staticFriction, float p_dynamicFriction, float p_restitution);
			~Friction();
			physx::PxMaterial* GetMaterial() const;
		private:
			physx::PxMaterial* m_material = nullptr;
		};
	}
}