#pragma once

#if defined (__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)) \
	            && defined (__GNUC_PATCHLEVEL__)

inline constexpr int FindFirstSet(unsigned int word)
{
    return __builtin_ffs(word) - 1;
}

inline constexpr int FindLastSet(unsigned int word)
{
    const int bit = word ? 32 - __builtin_clz(word) : 0;
    return bit - 1;
}

#elif defined (_MSC_VER) && (_MSC_VER >= 1400) && (defined (_M_IX86) || defined (_M_X64))
/* Microsoft Visual C++ support on x86/X64 architectures. */
#include <intrin.h>

#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanForward)

inline constexpr int FindFirstSet(unsigned int word)
{
    unsigned long index = 0;
    return _BitScanForward(&index, word) ? index : -1;
}

inline constexpr int FindLastSet(unsigned int word)
{
    unsigned long index = 0;
    return _BitScanReverse(&index, word) ? index : -1;
}
#else
/* Fall back to generic implementation. */
inline constexpr int GenericFindLastSet(unsigned int word)
{
    int bit = 32;

    if (!word) bit -= 1;
    if (!(word & 0xffff0000)) { word <<= 16; bit -= 16; }
    if (!(word & 0xff000000)) { word <<= 8; bit -= 8; }
    if (!(word & 0xf0000000)) { word <<= 4; bit -= 4; }
    if (!(word & 0xc0000000)) { word <<= 2; bit -= 2; }
    if (!(word & 0x80000000)) { word <<= 1; bit -= 1; }

    return bit;
}

/* Implement ffs in terms of fls. */
inline constexpr int FindFirstSet(unsigned int word)
{
    return GenericFindLastSet(word & (~word + 1)) - 1;
}

inline constexpr int FindLastSet(unsigned int word)
{
    return GenericFindLastSet(word) - 1;
}
#endif

inline constexpr int FindLastSet_sizet(size_t size)
{
    int high = (int)(size >> 32);
    int bits = 0;
    if (high)
    {
        bits = 32 + FindLastSet(high);
    }
    else
    {
        bits = FindLastSet((int)size & 0xffffffff);

    }
    return bits;
}