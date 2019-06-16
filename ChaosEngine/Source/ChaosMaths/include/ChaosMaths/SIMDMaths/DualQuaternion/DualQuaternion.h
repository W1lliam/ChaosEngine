#pragma once
#include "ChaosMaths/SIMDMaths/Quaternion/Quaternion.h"
#include "ChaosMaths/SIMDMaths/Vector/Vector3.h"

namespace  Chaos
{
	namespace Maths
	{
		_MM_ALIGN16 class DualQuaternion
		{
#pragma region Constructor& Destructor

		public:
			DualQuaternion();
			DualQuaternion(const Quaternion& p_nonDual, const Quaternion& p_dual);
			DualQuaternion(const __m128& p_nonDual, const __m128& p_dual);
			DualQuaternion(const Quaternion& p_rotation, const Vector4& p_translation);
			DualQuaternion(const Matrix4& p_toConvert);
			DualQuaternion(const DualQuaternion& p_other);
			DualQuaternion(const DualQuaternion&& p_other) noexcept;
			~DualQuaternion() = default;

			DualQuaternion&  operator=(const DualQuaternion& p_other);
			DualQuaternion&  operator=(DualQuaternion&& p_other) noexcept;

			static DualQuaternion Identity();
			static DualQuaternion FromRotationAndTranslation(const Quaternion& p_rotation, const Vector4& p_translation);

#pragma endregion 

#pragma region Arithmetic operators
			DualQuaternion operator+(const DualQuaternion& p_other) const;
			DualQuaternion operator-(const DualQuaternion& p_other) const;
			DualQuaternion operator*(const DualQuaternion& p_other) const;
			DualQuaternion operator*(float p_toMultiply) const;
			DualQuaternion operator/(float p_toDivide) const;

			DualQuaternion& operator+=(const DualQuaternion& p_other);
			DualQuaternion& operator-=(const DualQuaternion& p_other);
			DualQuaternion& operator*=(float p_toMultiply);
			DualQuaternion& operator*=(const DualQuaternion& p_other);
			DualQuaternion& operator/=(float p_toDivide);
#pragma endregion 

#pragma region Getter & Setter
			Quaternion& GetTranslationQuat();
			Quaternion& GetRotationQuat();
			Quaternion GetTranslation() const;
			Quaternion GetRotation() const;
			void SetRotationQuat(const Quaternion& p_rotation);
			void SetTranslationQuat(const Quaternion& p_translation);
#pragma endregion 

#pragma region Quaternion Operations
			void Normalized();
			DualQuaternion Normalize() const;
			DualQuaternion Conjugate() const;
			Vector4 Transform(const Vector4& p_toTransform) const;
			Vector4 Translate(const Vector4& p_toTranslate) const;
			Vector4 Rotate(const Vector4& p_toRotate) const;
			DualQuaternion Inverse() const;

#pragma endregion 
#pragma region Conversions
			Matrix4 To4x4Matrix();

#pragma endregion 

		private:
			Quaternion m_nonDual; //represents rotation
			Quaternion m_dual;    //represents translation
		};
	}
}