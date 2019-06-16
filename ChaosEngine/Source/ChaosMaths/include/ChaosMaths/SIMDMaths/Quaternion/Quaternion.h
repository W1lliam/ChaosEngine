#pragma once
#include "ChaosMaths/SIMDMaths/Matrix/Matrix4x4.h"
#include "ChaosMaths/SIMDMaths/Utilities.h"
#include "ChaosMaths/SIMDMaths/Vector/Vector3.h"
#include "ChaosMaths/SIMDMaths/Matrix/Matrix3x3.h"

namespace Chaos
{
	namespace Maths
	{
		_MM_ALIGN16 class Quaternion
		{
		public:
#pragma region Constructor& Destructor
			Quaternion();
			Quaternion(float p_x, float p_y, float p_z, float p_w);
			Quaternion(Vector4& p_quat);
			Quaternion(const Quaternion& p_other);
			explicit Quaternion(__m128& p_quaternion);
			~Quaternion() = default;

			Quaternion(const Quaternion&& p_other) noexcept;

			Quaternion& operator=(const Quaternion& p_other);
			Quaternion& operator=(Quaternion&& p_other) noexcept;
			static Quaternion Identity();

			explicit Quaternion(const Matrix4& p_matrix);

			void MakeFromEulerAngles(float p_xAngle, float p_yAngle, float p_zAngle, int p_xOrder, int p_yOrder, int p_zOrder);
			void MakeFromEulerAngles(Vector3 p_angles, int p_xOrder, int p_yOrder, int p_zOrder);
			void MakeFromEulerXYZ(float p_xAngle, float p_yAngle, float p_zAngle);
			void MakeFromEulerXZY(float p_xAngle, float p_yAngle, float p_zAngle);
			void MakeFromEulerYXZ(float p_xAngle, float p_yAngle, float p_zAngle);
			void MakeFromEulerYZX(float p_xAngle, float p_yAngle, float p_zAngle);
			void MakeFromEulerZXY(float p_xAngle, float p_yAngle, float p_zAngle);
			void MakeFromEulerZYX(float p_xAngle, float p_yAngle, float p_zAngle);
#pragma endregion 
#pragma region Arithmetic operators
#pragma region Add
			Quaternion operator+(const Quaternion& p_otherQuat) const;
			Quaternion& operator+=(const Quaternion& p_otherQuat);

#pragma endregion
#pragma region Substract
			Quaternion operator-(const Quaternion& p_otherQuat) const;
			Quaternion& operator-=(const Quaternion& p_otherQuat);

#pragma endregion
#pragma region Multiply
			float DotProduct(const Quaternion& p_otherQuat) const;
			static float DotProduct(const Quaternion& p_left, const Quaternion& p_right);
			float operator|(const Quaternion& p_otherQuat) const;

			Quaternion& operator*=(const float p_scale);
			Quaternion operator*(const float p_scale) const;

			Quaternion operator*(const Quaternion& p_otherQuat) const;
			Quaternion& operator*=(const Quaternion& p_otherQuat);
			static Quaternion FromMatrix4(const Matrix4& p_matrix);

#pragma endregion
#pragma region Divide
			Quaternion& operator/=(const float p_scale);
			Quaternion operator/(const float p_scale) const;

#pragma endregion
#pragma endregion 
#pragma region Getter & Setter
			__m128& GetElement();
			__m128 GetElement() const;
			float& GetRealValue();
			float& GetX();
			float& GetY();
			float& GetZ();

#pragma endregion 
			
#pragma region Quaternion Operations
			Quaternion& Normalize();
			Quaternion GetNormalized() const;
			static Quaternion Normalize(const Quaternion& p_quaternion);

			float Length() const;
			float LengthSquare() const;

			Quaternion& Inverse();
			static Quaternion Inverse(const Quaternion& p_quaternion);

			Quaternion& Conjugate();
			static Quaternion Conjugate(const Quaternion& p_quaternion);

			Quaternion& Square();
			static Quaternion Square(const Quaternion& p_quaternion);

			Vector3 ToEulerAngles() const;
			Vector3 ToEulerAnglesUsingForward() const;
			float AngularDistance(const Quaternion& p_other) const;

			static Quaternion LootAt(const Maths::Vector3 p_sourcePoint, const Maths::Vector3 p_targetPoint, const Maths::Vector3 p_cameraForward, const Maths::Vector3 p_cameraUp);
			/**
			 * @param p_first
			 * @param p_second
			 * @param p_alpha
			 * p_first is start of SLerp, p_second is the end of SLerp.
			 */
			static Quaternion Slerp(Quaternion& p_first, Quaternion& p_second, const float p_alpha);
			static Quaternion ApproximativeSlerp(Quaternion& p_first, Quaternion& p_second, const float p_alpha);
			static Quaternion NLerp(Quaternion& p_start, Quaternion& p_end, const float p_alpha);
			static Quaternion InverseQuat(const Quaternion& p_toInverse);
			/**
			* @param p_first
			* @param p_second
			* @param p_alpha
			* p_first is start of NLerp, p_second is the end of NLerp.
			*/

			Vector4 RotateAntiClockWise(Vector4 p_toRotate) const;
			Vector3 RotateAntiClockWise(Vector3 p_toRotate) const;
			Vector4 RotateClockWise(Vector4 p_toRotate) const;
			Vector3 RotateClockWise(Vector3 p_toRotate) const;

#pragma endregion 
#pragma region Conversions
			/**
			* Returning Euler axis angles for each axis.
			* First value of returned vector = X axis, second, Y axis, and third, Z axis.
			*/
			//Vector3<float> ToEuler();
			void ToString() const;
			static void ToString(const Quaternion& p_quaternion);
			//Matrix3<float> ToMatrix3() const;
			/**
			 * Convert Quaternion into 4x4 Matrix.
			 * It will be used as rotation matrix instead of euler axis rotation matrices.
			 */
			Matrix3 ToMatrix3() const;
			Matrix4 ToMatrix4() const;
#pragma endregion

		private:
			union
			{
				struct
				{
					//(X,Y,Z) Vector 
					//W Real
					float m_x, m_y, m_z, m_w;
				};
				float m_quatElements[4];
				__m128 m_element;
			};
		};
	}
}
