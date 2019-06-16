#pragma once
#include "ChaosMaths/SIMDMaths/Config/SIMDConfig.h"
#define PI 			(3.1415926535897932f)
#define DEG_TO_RAD  (3.1415926535897932f/180.0f)
#define RAD_TO_DEG  (180.0f/3.1415926535897932f)
namespace Chaos::Maths
{
	inline __m128 VecSin(__m128 p_vec)
	{
		const __m128 SinA = _mm_set1_ps(7.58946638440411f);
		const __m128 SinB = _mm_set1_ps(1.63384345775366f);
		__m128 x = _mm_mul_ps(p_vec, _mm_set1_ps(0.5f / PI));
		x = _mm_sub_ps(x, _mm_round_ps(_mm_add_ps(x, _mm_set1_ps(0.5f)), _MM_FROUND_FLOOR));
		__m128 y = _mm_mul_ps(SinA, _mm_mul_ps(x, _mm_sub_ps(_mm_set1_ps(0.5f), _mm_and_ps(x, _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF))))));
		return _mm_mul_ps(y, _mm_add_ps(_mm_and_ps(y, _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF))), SinB));
	}

	inline __m128 VecCos(__m128 p_vec)
	{
		return VecSin(_mm_add_ps(p_vec, _mm_set_ps1(PI * 0.5f)));
	}
}