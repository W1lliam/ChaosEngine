#pragma once

namespace Chaos
{
	namespace Physics
	{
		enum CollisionMask
		{
			C_STATIC = (1 << 0),
			C_DYNAMIC = (1 << 1),
			C_KINETIC = (1 << 2),
			SELF = (1 << 3),
			C_EMPTY = (1 << 4)
		};
	}
}