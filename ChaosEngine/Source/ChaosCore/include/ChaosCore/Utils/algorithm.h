#pragma once
#include <algorithm>
#include "ChaosCore/Multithreading/Containers/ThreadSafeMap.h"
#include "ChaosCore/ECS/Types.h"

namespace Chaos::Utils
{
	template<typename T>
	void depthFirstSearch(T p_vertex,
        std::unordered_map<SystemTypeID, int>& VERTEX_STATE,
		Containers::ThreadSafeMap<SystemTypeID, Containers::ThreadSafeMap<SystemTypeID, bool>*>& EDGES,
		std::vector<T>& output)
	{
		VERTEX_STATE[p_vertex] = 1; // visited
		for (auto& p_pair : VERTEX_STATE)
		{
            if (EDGES.at(p_pair.first)->exists(p_vertex) && EDGES.at(p_pair.first)->at(p_vertex) == true && p_pair.second == 0)
                depthFirstSearch<T>(p_pair.first, VERTEX_STATE, EDGES, output);
		}

		VERTEX_STATE[p_vertex] = 2; // done
		output.push_back(p_vertex);
	}

    inline constexpr int32_t PopCount(uint32_t value)
    {
        value = value - ((value >> 1) & 0x55555555);
        value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
        value = ((value + (value >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
        return ((int32_t)value);
    }

    inline constexpr int32_t Log2(uint32_t x)
    {
        uint32_t a = x | (x >> 1);
        uint32_t b = a | (a >> 2);
        uint32_t c = b | (b >> 4);
        uint32_t d = c | (c >> 8);
        uint32_t e = d | (d >> 16);
        uint32_t f = e >> 1;
        return PopCount(f);
    }

    inline constexpr int32_t BitsRequired(int32_t min, int32_t max)
    {
        return (min == max) ? 1 : Log2((uint32_t)(max - min)) + 1;
    }


    //Non constexpr for portability
    inline bool IsBigEndian(void) noexcept
    {
        union {
            uint32_t i;
            char c[4];
        } bint = { 0x01020304 };

        return bint.c[0] == 1;
    }


    template <typename T>
    T htonT(T p_value) noexcept
    {
        if (!IsBigEndian())
        {
            char* ptr = reinterpret_cast<char*>(&p_value);
            std::reverse(ptr, ptr + sizeof(T));
        }
        return p_value;
    }
}