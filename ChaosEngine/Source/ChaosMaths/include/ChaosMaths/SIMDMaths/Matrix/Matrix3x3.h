#pragma once
#include "ChaosMaths/SIMDMaths/Vector/Vector2.h"
#include "ChaosMaths/SIMDMaths/Vector/Vector3.h"
#include "MatrixUtilities.h"
#include <sstream>

namespace Chaos
{
	namespace Maths
	{
		struct Vector4;
		_MM_ALIGN16 struct Matrix3
		{
			union
			{
				struct
				{
					float m00, m01, m02, mEmpty1,
					      m10, m11, m12, mEmpty2,
					      m20, m21, m22, mEmpty3;
				};
				float elements[12];
				__m128 simdElements[3];
			};
#pragma region Constructor

			inline Matrix3();

			inline Matrix3(float p00, float p01, float p02,
			               float p10, float p11, float p12, 
			               float p20, float p21, float p22 
			);

			inline Matrix3(Vector3& p_firstRow,
			               Vector3& p_secondRow,
			               Vector3& p_lastRow);

			inline Matrix3(__m128& p_firstRow,
			               __m128& p_secondRow,
			               __m128& p_lastRow);

			static inline Matrix3 Identity();
#pragma endregion
#pragma region Arithmetic operators
			inline Matrix3 operator+(const Matrix3& p_other) const;
			inline Matrix3 operator-(const Matrix3& p_other) const;
			inline Matrix3 operator*(const Matrix3& p_other) const;
			inline Vector3 operator*(const Vector3& p_other) const;
			inline Matrix3 operator/(const Matrix3& p_other) const;
			inline Matrix3 operator*(float p_toMultiply) const;
			inline Matrix3 operator/(float p_toDivide) const;
#pragma endregion 
#pragma region Arithmetic and Assignment operators
			inline Matrix3 operator+=(const Matrix3& p_other);
			inline Matrix3 operator-=(const Matrix3& p_other);
			inline Matrix3 operator*=(const Matrix3& p_other);
			inline Matrix3 operator/=(const Matrix3& p_other);
			inline Matrix3 operator*=(float p_toMultiply);
			inline Matrix3 operator/=(float p_toDivide);
#pragma endregion 
#pragma region Equality operators
			inline bool operator==(const Matrix3& p_other);
			inline bool operator!=(const Matrix3& p_other);

#pragma endregion 
#pragma region Array access operators
			inline float& operator()(uint8_t p_row, uint8_t p_column);
#pragma endregion 
#pragma region Mathematic Methods
			static Matrix3 Translation(const Vector2& p_translation);
			static Matrix3 Rotation(const float p_angle);
			static Matrix3 Scale(const Vector2& p_scaling);
			inline Matrix3 Transpose() const;
			inline void Transposed();
			inline Matrix3 Inverse() const;
			inline void Inversed();
			inline float Determinant() const;
			inline float SimpleDeterminant() const;
			inline bool IsOrthogonal() const;
#pragma endregion 
#pragma region Console Display Methods
			std::string ToString() const;
#pragma endregion 
		};
	}
}
#include "Matrix3x3.inl"