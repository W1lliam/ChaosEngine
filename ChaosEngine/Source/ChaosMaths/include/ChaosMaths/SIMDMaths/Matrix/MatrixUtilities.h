#pragma once
#include "ChaosMaths/SIMDMaths/Config/SIMDConfig.h"
#define Shuffle(x,y,z,w) (x | (y<<2) | (z<<4) | (w<<6))
#define EPSILON 0.00001f
//adj(A)*B
inline __m128 AdjMat2MultiplicationMat2(__m128 mat2x2a, __m128 mat2x2b)
{
	return
		_mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(mat2x2a, mat2x2a, Shuffle(3, 3, 0, 0)), mat2x2b),
		_mm_mul_ps(_mm_shuffle_ps(mat2x2a, mat2x2a, Shuffle(1, 1, 2, 2)), _mm_shuffle_ps(mat2x2b, mat2x2b, Shuffle(2, 3, 0, 1))));
}
//A * adj(B)
inline __m128 Mat2MultiplicationWithAdjMat2(__m128 mat2x2a, __m128 mat2x2b)
{
	return
		_mm_sub_ps(_mm_mul_ps(mat2x2a, _mm_shuffle_ps(mat2x2b, mat2x2b, Shuffle(3, 0, 3, 0))),
		_mm_mul_ps(_mm_shuffle_ps(mat2x2a, mat2x2a, Shuffle(1, 0, 3, 2)), _mm_shuffle_ps(mat2x2b, mat2x2b, Shuffle(2, 1, 2, 1))));
}
//A * B
inline __m128 Mat2MultiplicationMat2(__m128 mat2x2a, __m128 mat2x2b)
{
	return
		_mm_add_ps(_mm_mul_ps(mat2x2a, _mm_shuffle_ps(mat2x2b, mat2x2b, Shuffle(0,3,0,3))),
		_mm_mul_ps(_mm_shuffle_ps(mat2x2a, mat2x2a, Shuffle(1,0,3,2)), _mm_shuffle_ps(mat2x2b, mat2x2b, Shuffle(2,1,2,1))));
}