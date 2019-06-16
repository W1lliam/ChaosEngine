#pragma once
#include "ChaosMaths/SIMDMaths/Config/SIMDConfig.h"

namespace Chaos
{
	namespace Maths
	{
		inline void* AlignedMemoryAllocation(size_t p_size, size_t p_align)
		{
#if defined (CHAOS_PLATFORM_WINDOWS)
			return _aligned_malloc(p_size, p_align);
#elif defined (CHAOS_PLATFORM_UNIX)
			return memalign(p_align, p_size);
#else
			return _mm_malloc(p_align, p_size);
#endif
		}

		inline void AlignedMemoryFree(void* p_toDelete)
		{
#if defined (CHAOS_PLATFORM_WINDOWS)
			return _aligned_free(p_toDelete);
#else
			std::free(p_toDelete);
#endif
		}
	}
}