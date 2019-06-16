#pragma once
#include "PxPhysicsAPI.h"
#include <ChaosMaths.h>
namespace Chaos
{
	namespace Physics
	{
		class Foundation
		{
		public:
			Foundation() = default;
			~Foundation();
			physx::PxFoundation* GetFoundation() const;
			void InitFoundation();
			void DestroyFoundation();

		private:
			physx::PxFoundation* m_foundation = nullptr;
			physx::PxDefaultAllocator m_allocator;
			physx::PxDefaultErrorCallback m_errorCallback;
		};
	}
}