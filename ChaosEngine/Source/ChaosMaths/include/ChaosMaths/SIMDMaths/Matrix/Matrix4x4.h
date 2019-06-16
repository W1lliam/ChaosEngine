#pragma once
#include "ChaosMaths/SIMDMaths/Matrix/MatrixUtilities.h"
#include "ChaosMaths/SIMDMaths/Utilities.h"
#include "ChaosMaths/SIMDMaths/Vector/Vector3.h"
#include "ChaosMaths/SIMDMaths/Vector/Vector4.h"
#include <sstream>
//Methods that are related to 2D(such as 2d transformation) and Matrix3x3 are implemented in Mat3x3.inl)
//Methods that are related to Vectors are implemented in MatVec.inl)


namespace Chaos
{
	namespace Maths
	{
		_MM_ALIGN16 struct Matrix4
		{
			union
			{
				struct
				{
					float m00, m01, m02, m03,
						  m10, m11, m12, m13,
						  m20, m21, m22, m23,
						  m30, m31, m32, m33;
				};
				float elements[16];
				__m128 simdElements[4];
			};
#pragma region Constructor

			inline Matrix4();
			
			inline Matrix4(float p00, float p01, float p02, float p03,
						   float p10, float p11, float p12, float p13,
						   float p20, float p21, float p22, float p23,
						   float p30, float p31, float p32, float p33);
			
			inline Matrix4(Vector4& p_firstRow,
						   Vector4& p_secondRow,
						   Vector4& p_thirdRow,
						   Vector4& p_lastRow);

			inline Matrix4(__m128& p_firstRow,
						   __m128& p_secondRow,
						   __m128& p_thirdRow,
						   __m128& p_lastRow);

			static inline Matrix4 Identity();
#pragma endregion
#pragma region Arithmetic operators
			inline Matrix4 operator+(const Matrix4& p_other) const;
			inline Matrix4 operator-(const Matrix4& p_other) const;
			inline Matrix4 operator*(const Matrix4& p_other) const;
			inline Vector4 operator*(const Vector4& p_other) const;
			inline Matrix4 operator/(const Matrix4& p_other) const;
			inline Matrix4 operator*(float p_toMultiply) const;
			inline Matrix4 operator/(float p_toDivide) const;
#pragma endregion 
#pragma region Arithmetic and Assignment operators
			inline Matrix4 operator+=(const Matrix4& p_other);
			inline Matrix4 operator-=(const Matrix4& p_other);
			inline Matrix4 operator*=(const Matrix4& p_other);
			inline Matrix4 operator/=(const Matrix4& p_other);
			inline Matrix4 operator*=(float p_toMultiply);
			inline Matrix4 operator/=(float p_toDivide);
#pragma endregion 
#pragma region Equality operators
			inline bool operator==(const Matrix4& p_other);
			inline bool operator!=(const Matrix4& p_other);

#pragma endregion 
#pragma region Array access operators
			inline float& operator()(uint8_t p_row, uint8_t p_column);
#pragma endregion 
#pragma region Mathematic Methods
			static Matrix4 Translation(const Vector3& p_translation);
			static Matrix4 RotationX(const float p_angle);
			static Matrix4 RotationY(const float p_angle);
			static Matrix4 RotationZ(const float p_angle);
			static Matrix4 RotationXYZ(const Vector3& p_angles);
			static Matrix4 Scale(const Vector3& p_scaling);
			inline Matrix4 Transpose() const;
			inline void Transposed();
			inline Matrix4 Inverse() const;
			inline void Inversed();
			inline float Determinant() const;
			inline bool IsOrthogonal() const;
			inline Vector3 GetCameraPosition(Matrix4& p_viewMatrix);
#pragma endregion 
#pragma region Console Display Methods
			std::string ToString() const;
#pragma endregion 

			static Matrix4 CreatePerspective(const float p_fov, const float p_aspectRatio, const float p_zNear, const float p_zFar);
			static Matrix4 CreateOrthographic(const float p_top, const float p_aspectRatio, const float p_zNear, const float p_zFar);
			static Matrix4 CreateLookAt(const float p_eyeX, const float p_eyeY, const float p_eyeZ, const float p_lookX, const float p_lookY, const float p_lookZ, const float p_upX, const float p_upY, const float p_upZ);
			static Matrix4 CreateFrustum(const float p_left, const float p_right, const float p_bottom, const float p_top, const float p_zNear, const float p_zFar);
		};
	}
}
#include "Matrix4x4.inl"