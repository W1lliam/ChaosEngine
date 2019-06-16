#pragma once
#include "ChaosMaths/SIMDMaths/Memory/SIMDAlignedMemory.h"
#include <cstdint>
#include <iostream>
namespace Chaos
{
	namespace Maths
	{
		_MM_ALIGN16 struct Vector3
		{
            static const Vector3 Zero;
            static const Vector3 Up;
            static const Vector3 Front;
            static const Vector3 Right;

			union
			{
				struct { float x, y, z, empty; };
				float elements[4];
				__m128 simdVector;
			};

#pragma region Constructor
			 Vector3();
			 Vector3(float x, float y, float z);
			 Vector3(__m128 p_simdVector);
#pragma  endregion 

#pragma region Aligned Memory Management
#pragma endregion 

#pragma region Arithmetic operators
			 Vector3 operator+(const Vector3& p_other) const;
			 Vector3 operator-(const Vector3& p_other) const;
			 float operator*(const Vector3& p_other) const;

			 Vector3 operator*(float p_toMultiply) const;
			 Vector3 operator/(float p_toDivide) const;
#pragma endregion 

#pragma region Arithmetic and Assignment operators
			 Vector3 operator+=(const Vector3& p_other);
			 Vector3 operator-=(const Vector3& p_other);

			 Vector3 operator*=(float p_toMultiply);
			 Vector3 operator/=(float p_toDivide);
#pragma endregion 

#pragma region Equality operators
			 bool operator==(const Vector3& p_toCompare) const;
			 bool operator!=(const Vector3& p_toCompare) const;
#pragma endregion 

#pragma region Array access operators
			 const float& operator[](const uint8_t p_index) const;
			 float& operator[](const uint8_t p_index);
#pragma endregion 

#pragma region Mathematic Methods
			 Vector3 Cross(const Vector3& p_other) const;
			 float Dot(const Vector3& p_other) const;
			 float Length() const;
			 Vector3 Normalized() const;
			 void Normalize();
			 static Vector3 Lerp(Vector3& p_start, Vector3& p_end, float p_alpha);
#pragma endregion 

			 std::string ToString();
		};
#pragma region Console Display Methods
		  std::ostream& operator<<(std::ostream& p_stream, const Vector3& p_vector);
#pragma endregion
	}


}