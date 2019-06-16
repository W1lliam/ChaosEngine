#pragma once
#include "ChaosMaths/SIMDMaths/Memory/SIMDAlignedMemory.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
namespace Chaos
{
	namespace Maths 
	{
		_MM_ALIGN16 struct Vector4
		{
			union
			{
				struct { float x, y, z, w; };
				float elements[4];
				__m128 simdVector;
			};

#pragma region Constructor
			inline Vector4();
			inline Vector4(float x, float y, float z, float w);
			inline Vector4(__m128 p_simdVector);
#pragma  endregion 

#pragma region Aligned Memory Management
			inline void* operator new[](size_t p_size);
			inline void operator delete[](void* p_toDelete);
#pragma endregion 

#pragma region Arithmetic operators
			inline Vector4 operator+(const Vector4& p_other) const;
			inline Vector4 operator-(const Vector4& p_other) const;
			inline float operator*(const Vector4& p_other) const;

			inline Vector4 operator*(float p_toMultiply) const;
			inline Vector4 operator/(float p_toDivide) const;
#pragma endregion 

#pragma region Arithmetic and Assignment operators
			inline Vector4 operator+=(const Vector4& p_other);
			inline Vector4 operator-=(const Vector4& p_other);

			inline Vector4 operator*=(float p_toMultiply);
			inline Vector4 operator/=(float p_toDivide);
#pragma endregion 

#pragma region Equality operators
			inline bool operator==(const Vector4& p_toCompare) const;
			inline bool operator!=(const Vector4& p_toCompare) const;
#pragma endregion 
			
#pragma region Array access operators
			inline const float& operator[](const uint8_t p_index) const;
			inline float& operator[](const uint8_t p_index);
#pragma endregion 

#pragma region Mathematic Methods
			inline Vector4 Cross(const Vector4& p_other) const;
			inline float Dot(const Vector4& p_other) const;
			inline float Length() const;
			inline Vector4 Normalized() const;
			inline void Normalize();
			inline Vector4 Homogenized() const;
			inline void Homogenize();
#pragma endregion 
		};
#pragma region Console Display Methods
		 inline std::ostream& operator<<(std::ostream& p_stream, const Vector4& p_vector);
#pragma endregion
	}
}
#include "ChaosMaths/SIMDMaths/Vector/Vector4.inl"