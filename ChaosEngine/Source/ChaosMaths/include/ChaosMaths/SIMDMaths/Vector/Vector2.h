#pragma once
#include "ChaosMaths/SIMDMaths/Memory/SIMDAlignedMemory.h"
#include <cstdint>
#include <iostream>
namespace Chaos
{
	namespace Maths
	{
		_MM_ALIGN16 struct Vector2
		{
			union
			{
				struct { float x, y; };
				float elements[2];
				__m128 simdVector;
			};

#pragma region Constructor
			inline Vector2();
			inline Vector2(float x, float y);
			inline Vector2(__m128 p_simdVector);
#pragma  endregion 

#pragma region Aligned Memory Management
#pragma endregion 

#pragma region Arithmetic operators
			inline Vector2 operator+(const Vector2& p_other) const;
			inline Vector2 operator-(const Vector2& p_other) const;
			inline float operator*(const Vector2& p_other) const;

			inline Vector2 operator*(float p_toMultiply) const;
			inline Vector2 operator/(float p_toDivide) const;
#pragma endregion 

#pragma region Arithmetic and Assignment operators
			inline Vector2 operator+=(const Vector2& p_other);
			inline Vector2 operator-=(const Vector2& p_other);
			inline Vector2 operator*=(float p_toMultiply);
			inline Vector2 operator/=(float p_toDivide);
#pragma endregion 

#pragma region Equality operators
			inline bool operator==(const Vector2& p_toCompare) const;
			inline bool operator!=(const Vector2& p_toCompare) const;
#pragma endregion 

#pragma region Array access operators
			inline const float& operator[](const uint8_t p_index) const;
			inline float& operator[](const uint8_t p_index);
#pragma endregion 

#pragma region Mathematic Methods
			inline float Dot(const Vector2& p_other) const;
			inline float Length() const;
			inline Vector2 Normalized() const;
			inline void Normalize();
#pragma endregion 
		};
#pragma region Console Display Methods
		 inline std::ostream& operator<<(std::ostream& p_stream, const Vector2& p_vector);
#pragma endregion
	}
}
#include "ChaosMaths/SIMDMaths/Vector/Vector2.inl"