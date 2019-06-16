#pragma once
#include <cstdint>
#include "ChaosCore/Utils/StringID.h"

namespace Chaos
{
    #define INVALID_TYPEID StringID()
    using ObjectID          = uint64_t;
    using TypeID            = StringID;

	using EntityID          = StringID;
	using ComponentID       = ObjectID;
	using ComponentTypeID   = TypeID;
	using SystemTypeID      = TypeID;
}