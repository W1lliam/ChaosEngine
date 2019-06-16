#pragma once
#include <cstdint>
#include "ChaosCore/ECS/Types.h"

namespace Chaos::Utils
{
	template<typename T>
	class FamilyTypeID
	{
	public:
        FamilyTypeID() = delete;
        ~FamilyTypeID() = delete;
		template<typename U>
		inline static ObjectID Get()
		{
			static const ObjectID STATIC_TYPE_ID{ s_count++ };
			return STATIC_TYPE_ID;
		}

        inline static ObjectID Get()
		{
			return s_count;
		}
	private:
		static ObjectID s_count;
	};

	template<typename T>
	class FamilyObjectID
	{
	public:
        FamilyObjectID() = delete;
        ~FamilyObjectID() = delete;

        inline constexpr static ObjectID Get()
		{
			return s_count++;
		}
    private:
        static ObjectID s_count;
	};


    template<typename T>
    ObjectID FamilyTypeID<T>::s_count = 0;
    template<typename T>
    ObjectID FamilyObjectID<T>::s_count = 0;
}