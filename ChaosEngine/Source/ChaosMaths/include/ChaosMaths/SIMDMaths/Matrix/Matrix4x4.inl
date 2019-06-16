#include "Matrix4x4.h"
namespace Chaos
{
	namespace Maths
	{
#pragma region Constructor

		inline Matrix4::Matrix4()
		{
			simdElements[0] = _mm_setr_ps(1.0f, 0, 0, 0);
			simdElements[1] = _mm_setr_ps(0, 1.0f, 0, 0);
			simdElements[2] = _mm_setr_ps(0, 0, 1.0f, 0);
			simdElements[3] = _mm_setr_ps(0, 0, 0, 1.0f);
		}
		inline Matrix4::Matrix4(float p00, float p01, float p02, float p03, float p10, float p11, float p12, float p13, float p20, float p21, float p22, float p23, float p30, float p31, float p32, float p33)
		{
			simdElements[0] = _mm_setr_ps(p00, p01, p02, p03);
			simdElements[1] = _mm_setr_ps(p10, p11, p12, p13);
			simdElements[2] = _mm_setr_ps(p20, p21, p22, p23);
			simdElements[3] = _mm_setr_ps(p30, p31, p32, p33);
		}
		inline Matrix4::Matrix4(__m128& p_firstRow, __m128& p_secondRow, __m128& p_thirdRow, __m128& p_lastRow)
		{
			simdElements[0] = p_firstRow;
			simdElements[1] = p_secondRow;
			simdElements[2] = p_thirdRow;
			simdElements[3] = p_lastRow;
		}
		inline Matrix4::Matrix4(Vector4& p_firstRow, Vector4& p_secondRow, Vector4& p_thirdRow, Vector4& p_lastRow)
		{
			simdElements[0] = p_firstRow.simdVector;
			simdElements[1] = p_secondRow.simdVector;
			simdElements[2] = p_thirdRow.simdVector;
			simdElements[3] = p_lastRow.simdVector;
		}
		inline Matrix4 Matrix4::Identity()
		{
			return Matrix4();
		}
#pragma  endregion 
#pragma region Arithmetic operators
		inline Matrix4 Matrix4::operator+(const Matrix4& p_other) const
		{
			Matrix4 add;
			add.simdElements[0] = _mm_add_ps(simdElements[0], p_other.simdElements[0]);
			add.simdElements[1] = _mm_add_ps(simdElements[1], p_other.simdElements[1]);
			add.simdElements[2] = _mm_add_ps(simdElements[2], p_other.simdElements[2]);
			add.simdElements[3] = _mm_add_ps(simdElements[3], p_other.simdElements[3]);
			return add;
		}
		inline Matrix4 Matrix4::operator-(const Matrix4& p_other) const
		{
			Matrix4 sub;
			sub.simdElements[0] = _mm_sub_ps(simdElements[0], p_other.simdElements[0]);
			sub.simdElements[1] = _mm_sub_ps(simdElements[1], p_other.simdElements[1]);
			sub.simdElements[2] = _mm_sub_ps(simdElements[2], p_other.simdElements[2]);
			sub.simdElements[3] = _mm_sub_ps(simdElements[3], p_other.simdElements[3]);
			return sub;
		}
		inline Matrix4 Matrix4::operator/(float p_toDivide) const
		{
			Matrix4 div;
			__m128 scalar = _mm_set1_ps(p_toDivide);
			div.simdElements[0] = _mm_div_ps(simdElements[0], scalar);
			div.simdElements[1] = _mm_div_ps(simdElements[1], scalar);
			div.simdElements[2] = _mm_div_ps(simdElements[2], scalar);
			div.simdElements[3] = _mm_div_ps(simdElements[3], scalar);
			return div;
		}

		inline Vector4 Matrix4::operator*(const Vector4& p_other) const
		{
			__m128 first =  _mm_mul_ps(simdElements[0], p_other.simdVector);
			__m128 second = _mm_mul_ps(simdElements[1], p_other.simdVector);
			__m128 third =  _mm_mul_ps(simdElements[2], p_other.simdVector);
			__m128 fourth = _mm_mul_ps(simdElements[3], p_other.simdVector);

			return Vector4( _mm_cvtss_f32(_mm_dp_ps(first, _mm_set_ps1(1.0f), 0xff)),
							_mm_cvtss_f32(_mm_dp_ps(second, _mm_set_ps1(1.0f), 0xff)),
							_mm_cvtss_f32(_mm_dp_ps(third, _mm_set_ps1(1.0f), 0xff)),
							_mm_cvtss_f32(_mm_dp_ps(fourth, _mm_set_ps1(1.0f), 0xff)));
		}

		inline Matrix4 Matrix4::operator*(float p_toMultiply) const
		{
			Matrix4 mul;
			__m128 scalar = _mm_set1_ps(p_toMultiply);
			mul.simdElements[0] = _mm_mul_ps(simdElements[0], scalar);
			mul.simdElements[1] = _mm_mul_ps(simdElements[1], scalar);
			mul.simdElements[2] = _mm_mul_ps(simdElements[2], scalar);
			mul.simdElements[3] = _mm_mul_ps(simdElements[3], scalar);
			return mul;
		}
		inline Matrix4 Matrix4::operator*(const Matrix4& p_other) const
		{
			const __m128* vecA = p_other.simdElements;
			const __m128* vecB = simdElements;
			Matrix4 multiplied;
			// line 0
			multiplied.simdElements[0] = _mm_mul_ps(vecA[0], _mm_shuffle_ps(vecB[0], vecB[0], Shuffle(0,0,0,0)));
			multiplied.simdElements[0] = _mm_add_ps(multiplied.simdElements[0], _mm_mul_ps(vecA[1], _mm_shuffle_ps(vecB[0], vecB[0], Shuffle(1,1,1,1))));
			multiplied.simdElements[0] = _mm_add_ps(multiplied.simdElements[0], _mm_mul_ps(vecA[2], _mm_shuffle_ps(vecB[0], vecB[0], Shuffle(2,2,2,2))));
			multiplied.simdElements[0] = _mm_add_ps(multiplied.simdElements[0], _mm_mul_ps(vecA[3], _mm_shuffle_ps(vecB[0], vecB[0], Shuffle(3,3,3,3))));
			// line 1
			multiplied.simdElements[1] = _mm_mul_ps(vecA[0], _mm_shuffle_ps(vecB[1], vecB[1], Shuffle(0, 0, 0, 0)));
			multiplied.simdElements[1] = _mm_add_ps(multiplied.simdElements[1], _mm_mul_ps(vecA[1], _mm_shuffle_ps(vecB[1], vecB[1], Shuffle(1, 1, 1, 1))));
			multiplied.simdElements[1] = _mm_add_ps(multiplied.simdElements[1], _mm_mul_ps(vecA[2], _mm_shuffle_ps(vecB[1], vecB[1], Shuffle(2, 2, 2, 2))));
			multiplied.simdElements[1] = _mm_add_ps(multiplied.simdElements[1], _mm_mul_ps(vecA[3], _mm_shuffle_ps(vecB[1], vecB[1], Shuffle(3, 3, 3, 3))));
			// line 2
			multiplied.simdElements[2] = _mm_mul_ps(vecA[0], _mm_shuffle_ps(vecB[2], vecB[2], Shuffle(0, 0, 0, 0)));
			multiplied.simdElements[2] = _mm_add_ps(multiplied.simdElements[2], _mm_mul_ps(vecA[1], _mm_shuffle_ps(vecB[2], vecB[2], Shuffle(1, 1, 1, 1))));
			multiplied.simdElements[2] = _mm_add_ps(multiplied.simdElements[2], _mm_mul_ps(vecA[2], _mm_shuffle_ps(vecB[2], vecB[2], Shuffle(2, 2, 2, 2))));
			multiplied.simdElements[2] = _mm_add_ps(multiplied.simdElements[2], _mm_mul_ps(vecA[3], _mm_shuffle_ps(vecB[2], vecB[2], Shuffle(3, 3, 3, 3))));
			// line 3
			multiplied.simdElements[3] = _mm_mul_ps(vecA[0], _mm_shuffle_ps(vecB[3], vecB[3], Shuffle(0, 0, 0, 0)));
			multiplied.simdElements[3] = _mm_add_ps(multiplied.simdElements[3], _mm_mul_ps(vecA[1], _mm_shuffle_ps(vecB[3], vecB[3], Shuffle(1, 1, 1, 1))));
			multiplied.simdElements[3] = _mm_add_ps(multiplied.simdElements[3], _mm_mul_ps(vecA[2], _mm_shuffle_ps(vecB[3], vecB[3], Shuffle(2, 2, 2, 2))));
			multiplied.simdElements[3] = _mm_add_ps(multiplied.simdElements[3], _mm_mul_ps(vecA[3], _mm_shuffle_ps(vecB[3], vecB[3], Shuffle(3, 3, 3, 3))));
			return multiplied;
		}
		inline Matrix4 Matrix4::operator/(const Matrix4& p_other) const
		{
			const Matrix4 InverseOfOther = p_other.Inverse();
			return(*this * InverseOfOther);
		}
#pragma endregion 
#pragma region Arithmetic and Assignment operators
		inline Matrix4 Matrix4::operator+=(const Matrix4& p_other)
		{
			simdElements[0] = _mm_add_ps(simdElements[0], p_other.simdElements[0]);
			simdElements[1] = _mm_add_ps(simdElements[1], p_other.simdElements[1]);
			simdElements[2] = _mm_add_ps(simdElements[2], p_other.simdElements[2]);
			simdElements[3] = _mm_add_ps(simdElements[3], p_other.simdElements[3]);
			return *this;
		}
		inline Matrix4 Matrix4::operator-=(const Matrix4& p_other)
		{
			simdElements[0] = _mm_sub_ps(simdElements[0], p_other.simdElements[0]);
			simdElements[1] = _mm_sub_ps(simdElements[1], p_other.simdElements[1]);
			simdElements[2] = _mm_sub_ps(simdElements[2], p_other.simdElements[2]);
			simdElements[3] = _mm_sub_ps(simdElements[3], p_other.simdElements[3]);
			return *this;
		}
		inline Matrix4 Matrix4::operator*=(const Matrix4& p_other)
		{
			*this = *this * p_other;
			return *this;
		}
		inline Matrix4 Matrix4::operator/=(const Matrix4& p_other)
		{
			const Matrix4 InverseOfOther = p_other.Inverse();
			*this = (*this * InverseOfOther);
			return *this;
		}

		inline Matrix4 Matrix4::operator*=(float p_toMultiply)
		{
			__m128 scalar = _mm_set1_ps(p_toMultiply);
			simdElements[0] = _mm_mul_ps(simdElements[0], scalar);
			simdElements[1] = _mm_mul_ps(simdElements[1], scalar);
			simdElements[2] = _mm_mul_ps(simdElements[2], scalar);
			simdElements[3] = _mm_mul_ps(simdElements[3], scalar);
			return *this;
		}
		inline Matrix4 Matrix4::operator/=(float p_toDivide)
		{
			__m128 scalar = _mm_set1_ps(p_toDivide);
			simdElements[0] = _mm_div_ps(simdElements[0], scalar);
			simdElements[1] = _mm_div_ps(simdElements[1], scalar);
			simdElements[2] = _mm_div_ps(simdElements[2], scalar);
			simdElements[3] = _mm_div_ps(simdElements[3], scalar);
			return *this;
		}

		inline bool Matrix4::operator==(const Matrix4 & p_other)
		{
			__m128 line;
			int lineMask;
			for (int i = 0; i < 4; ++i)
			{
				line = _mm_cmpeq_ps(simdElements[i], p_other.simdElements[i]);
				lineMask = _mm_movemask_ps(line);
				if (lineMask != 15)
				{
					return false;
				}
			}
			return true;
		}
		inline bool Matrix4::operator!=(const Matrix4 & p_other)
		{
			return *this == p_other ? false : true;
		}
#pragma endregion 
#pragma region Mathematic Methods

		inline float& Matrix4::operator()(uint8_t p_row, uint8_t p_column)
		{
			if (p_row >= 4 || p_column >= 4)
				throw std::out_of_range(
					"Invalid index : " + std::to_string(p_row) + "," + std::to_string(p_column) + " is out of range");
			return elements[4 * p_row + p_column];
		}

		inline Matrix4 Matrix4::Translation(const Vector3 & p_translation)
		{
			return Matrix4(_mm_setr_ps(1.0f, 0, 0, p_translation.x),
						   _mm_setr_ps(1.0f, 0, 0, p_translation.y),
						   _mm_setr_ps(1.0f, 0, 0, p_translation.z),
						   _mm_setr_ps(0, 0, 0, 1.f));
		}

		inline Matrix4 Matrix4::RotationX(const float p_angle)
		{
			float cos = _mm_cvtss_f32(VecCos(_mm_set_ps1(p_angle)));
			float sin = _mm_cvtss_f32(VecSin(_mm_set_ps1(p_angle)));
			return Matrix4(1, 0, 0, 0,
						   0, cos, -sin, 0,
						   0, sin, cos, 0,
						   0, 0, 0, 1);

		}

		inline Matrix4 Matrix4::RotationY(const float p_angle)
		{
			float cos = _mm_cvtss_f32(VecCos(_mm_set_ps1(p_angle)));
			float sin = _mm_cvtss_f32(VecSin(_mm_set_ps1(p_angle)));
			return Matrix4(cos, 0, -sin, 0,
						   0, 1, 0, 0,
						   sin, 0, cos, 0,
						   0, 0, 0, 1);
		}

		inline Matrix4 Matrix4::RotationZ(const float p_angle)
		{
			float cos = _mm_cvtss_f32(VecCos(_mm_set_ps1(p_angle)));
			float sin = _mm_cvtss_f32(VecSin(_mm_set_ps1(p_angle)));
			return Matrix4(cos, -sin, 0, 0,
						   sin, cos, 0, 0,
						   0, 0, 1, 0,
						   0, 0, 0, 1);
		}

		inline Matrix4 Matrix4::RotationXYZ(const Vector3 & p_angles)
		{
			return RotationX(p_angles.x) * RotationY(p_angles.y) * RotationZ(p_angles.z);
		}

		inline Matrix4 Matrix4::Scale(const Vector3 & p_scaling)
		{
			return Matrix4(_mm_setr_ps(p_scaling.x, 0, 0,0),
						   _mm_setr_ps(0, p_scaling.y, 0, 0),
						   _mm_setr_ps(0, 0, p_scaling.z, 0),
						   _mm_setr_ps(0, 0, 0, 1.f));
		}

		inline Matrix4 Matrix4::Transpose() const
		{
			Matrix4 transposed;
			__m128 t0 = _mm_movelh_ps(simdElements[0], simdElements[1]); // 00, 01, 10, 11
			__m128 t1 = _mm_movelh_ps(simdElements[2], simdElements[3]); // 20, 21, 30, 31
			__m128 t2 = _mm_movehl_ps(simdElements[0], simdElements[1]); // 02, 03, 12, 13
			__m128 t3 = _mm_movehl_ps(simdElements[2], simdElements[3]); // 22, 23, 32, 33

			transposed.simdElements[0] = _mm_shuffle_ps(t0, t1, Shuffle(0, 2, 0, 2)); // 00, 10, 20, 30
			transposed.simdElements[1] = _mm_shuffle_ps(t0, t1, Shuffle(1, 3, 1, 3)); // 01, 11, 21, 31
			transposed.simdElements[2] = _mm_shuffle_ps(t2, t3, Shuffle(2, 0, 2, 0)); // 02, 12, 22, 32
			transposed.simdElements[3] = _mm_shuffle_ps(t2, t3, Shuffle(3, 1, 3, 1)); // 03, 13, 23, 33

			return transposed;
		}
		inline void Matrix4::Transposed()
		{
			__m128 t0 = _mm_movelh_ps(simdElements[0], simdElements[1]); // 00, 01, 10, 11
			__m128 t1 = _mm_movelh_ps(simdElements[2], simdElements[3]); // 20, 21, 30, 31
			__m128 t2 = _mm_movehl_ps(simdElements[0], simdElements[1]); // 02, 03, 12, 13
			__m128 t3 = _mm_movehl_ps(simdElements[2], simdElements[3]); // 22, 23, 32, 33

			simdElements[0] = _mm_shuffle_ps(t0, t1, Shuffle(0, 2, 0, 2)); // 00, 10, 20, 30
			simdElements[1] = _mm_shuffle_ps(t0, t1, Shuffle(1, 3, 1, 3)); // 01, 11, 21, 31
			simdElements[2] = _mm_shuffle_ps(t2, t3, Shuffle(2, 0, 2, 0)); // 02, 12, 22, 32
			simdElements[3] = _mm_shuffle_ps(t2, t3, Shuffle(3, 1, 3, 1)); // 03, 13, 23, 33
		}
		inline float Matrix4::Determinant() const
		{
			// sub matrices
			__m128 A = _mm_movelh_ps(simdElements[0], simdElements[1]);
			__m128 B = _mm_movehl_ps(simdElements[1], simdElements[0]);
			__m128 C = _mm_movelh_ps(simdElements[2], simdElements[3]);
			__m128 D = _mm_movehl_ps(simdElements[3], simdElements[2]);

			float detA = (m00 * m11 - m01 * m10);
			float detB = (m02 * m13 - m03 * m12);
			float detC = (m20 * m31 - m21 * m30);
			float detD = (m22 * m33 - m23 * m32);

			// A#B
			__m128 adjAXB = AdjMat2MultiplicationMat2(A, B);
			// D#C
			__m128 adjDXC = AdjMat2MultiplicationMat2(D, C);
			Vector4 trace(Mat2MultiplicationMat2(adjAXB, adjDXC));
			// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
			return detA * detD + detB * detC - (trace.x + trace.w);
		}
		inline bool Matrix4::IsOrthogonal() const
		{
			Matrix4 identity;
			return identity == (*this * this->Transpose()) ? true : false;
		}
		inline Vector3 Matrix4::GetCameraPosition(Matrix4 & p_viewMatrix)
		{
			Matrix4 world = p_viewMatrix.Inverse();
			return Vector3(world.m30, world.m31, world.m32);
		}
		inline std::string Matrix4::ToString() const
		{
			std::stringstream ss;
			ss << elements[0] << "\t" << elements[1] << "\t" << elements[2] << "\t" << elements[3] << "\n" <<
				elements[4] << "\t" << elements[5] << "\t" << elements[6] << "\t" << elements[7] << "\n" <<
				elements[8] << "\t" << elements[9] << "\t" << elements[10] << "\t" << elements[11] << "\n" <<
				elements[12] << "\t" << elements[13] << "\t" << elements[14] << "\t" << elements[15] << "\n";
			std::string result = ss.str();
			return result;
		}
		inline Matrix4 Matrix4::Inverse() const
		{
			if(IsOrthogonal())
			{
				return Transpose();
			}
			
			Vector4 A = _mm_movelh_ps(simdElements[0], simdElements[1]);
			Vector4 B = _mm_movehl_ps(simdElements[1], simdElements[0]);
			Vector4 C = _mm_movelh_ps(simdElements[2], simdElements[3]);
			Vector4 D = _mm_movehl_ps(simdElements[3], simdElements[2]);
			//Sub Matrix determinants
			float detA = (m00 * m11 - m01 * m10);
			float detB = (m02 * m13 - m03 * m12);
			float detC = (m20 * m31 - m21 * m30);
			float detD = (m22 * m33 - m23 * m32);
			//D#C 
			//__m128 adjDXC = 

			// let iM = 1/|M| * | X  Y |
		//                  | Z  W |

		// D#C
			__m128 adjDXC = AdjMat2MultiplicationMat2(D.simdVector, C.simdVector);
			// A#B
			__m128 adjAXB = AdjMat2MultiplicationMat2(A.simdVector, B.simdVector);
			// X# = |D|A - B(D#C)
			__m128 X = _mm_sub_ps((A * detD).simdVector, Mat2MultiplicationMat2(B.simdVector, adjDXC));
			// W# = |A|D - C(A#B)
			__m128 W = _mm_sub_ps((D * detA).simdVector, Mat2MultiplicationMat2(C.simdVector, adjAXB));

			// Y# = |B|C - D(A#B)#
			__m128 Y = _mm_sub_ps((C * detB).simdVector, Mat2MultiplicationWithAdjMat2(D.simdVector, adjAXB));
			// Z# = |C|B - A(D#C)#
			__m128 Z = _mm_sub_ps((B * detC).simdVector, Mat2MultiplicationWithAdjMat2(A.simdVector, adjDXC));

			const Vector4 SignMask(1.f, -1.f, -1.f, 1.f);

			// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
			float DetM = Determinant();
			Vector4 rDetM = SignMask / DetM;

			X = _mm_mul_ps(X, rDetM.simdVector);
			Y = _mm_mul_ps(Y, rDetM.simdVector);
			Z = _mm_mul_ps(Z, rDetM.simdVector);
			W = _mm_mul_ps(W, rDetM.simdVector);

			Matrix4 inverse;

			// apply adjugate and store, here we combine adjugate shuffle and store shuffle
			inverse.simdElements[0] = _mm_shuffle_ps(X, Y, Shuffle(3, 1, 3, 1));
			inverse.simdElements[1] = _mm_shuffle_ps(X, Y, Shuffle(2, 0, 2, 0));
			inverse.simdElements[2] = _mm_shuffle_ps(Z, W, Shuffle(3, 1, 3, 1));
			inverse.simdElements[3] = _mm_shuffle_ps(Z, W, Shuffle(2, 0, 2, 0));

			return inverse;
		}
		inline void Matrix4::Inversed()
		{
			if(IsOrthogonal())
			{
				Transposed();
			}
			else
			{
				float DetM = Determinant();
				//Sub Matrices
				Vector4 A = _mm_movelh_ps(simdElements[0], simdElements[1]);
				Vector4 B = _mm_movehl_ps(simdElements[1], simdElements[0]);
				Vector4 C = _mm_movelh_ps(simdElements[2], simdElements[3]);
				Vector4 D = _mm_movehl_ps(simdElements[3], simdElements[2]);
				//Sub Matrix determinants
				float detA = (m00 * m11 - m01 * m10);
				float detB = (m02 * m13 - m03 * m12);
				float detC = (m20 * m31 - m21 * m30);
				float detD = (m22 * m33 - m23 * m32);
				//D#C 
				//__m128 adjDXC = 

				// let iM = 1/|M| * | X  Y |
			//                  | Z  W |

			// D#C
				__m128 adjDXC = AdjMat2MultiplicationMat2(D.simdVector, C.simdVector);
				// A#B
				__m128 adjAXB = AdjMat2MultiplicationMat2(A.simdVector, B.simdVector);
				// X# = |D|A - B(D#C)
				__m128 X = _mm_sub_ps((A * detD).simdVector, Mat2MultiplicationMat2(B.simdVector, adjDXC));
				// W# = |A|D - C(A#B)
				__m128 W = _mm_sub_ps((D * detA).simdVector, Mat2MultiplicationMat2(C.simdVector, adjAXB));

				// Y# = |B|C - D(A#B)#
				__m128 Y = _mm_sub_ps((C * detB).simdVector, Mat2MultiplicationWithAdjMat2(D.simdVector, adjAXB));
				// Z# = |C|B - A(D#C)#
				__m128 Z = _mm_sub_ps((B * detC).simdVector, Mat2MultiplicationWithAdjMat2(A.simdVector, adjDXC));

				const Vector4 SignMask(1.f, -1.f, -1.f, 1.f);

				// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
				Vector4 rDetM = SignMask / DetM;

				X = _mm_mul_ps(X, rDetM.simdVector);
				Y = _mm_mul_ps(Y, rDetM.simdVector);
				Z = _mm_mul_ps(Z, rDetM.simdVector);
				W = _mm_mul_ps(W, rDetM.simdVector);

				// apply adjugate and store, here we combine adjugate shuffle and store shuffle
				simdElements[0] = _mm_shuffle_ps(X, Y, Shuffle(3, 1, 3, 1));
				simdElements[1] = _mm_shuffle_ps(X, Y, Shuffle(2, 0, 2, 0));
				simdElements[2] = _mm_shuffle_ps(Z, W, Shuffle(3, 1, 3, 1));
				simdElements[3] = _mm_shuffle_ps(Z, W, Shuffle(2, 0, 2, 0));
			}
		}
#pragma endregion 

        inline Matrix4 Matrix4::CreatePerspective(const float p_fov, const float p_aspectRatio, const float p_zNear, const float p_zFar)
        {
            const float height = p_zNear * tanf(p_fov * 0.5f * DEG_TO_RAD);
            const float width = height * p_aspectRatio;
            return CreateFrustum(-width, width, -height, height, p_zNear, p_zFar);
        }

        inline Matrix4 Matrix4::CreateOrthographic(const float p_top, const float p_aspectRatio, const float p_zNear, const float p_zFar)
        {
            Chaos::Maths::Matrix4 Frustum;
            Frustum.m00 = 1 / (p_top * p_aspectRatio);
            Frustum.m11 = 1 / p_top;
            Frustum.m22 = -2 / (p_zFar - p_zNear);
            Frustum.m23 = -(p_zFar + p_zNear) / (p_zFar - p_zNear);
            return Frustum;
        }

		/*inline Matrix4 Matrix4::CreateLookAt(const float p_eyeX, const float p_eyeY, const float p_eyeZ, const float p_lookX, const float p_lookY, const float p_lookZ, const float p_upX, const float p_upY, const float p_upZ)
		{
			Chaos::Maths::Vector3 eye(p_eyeX, p_eyeY, p_eyeZ);
			Chaos::Maths::Vector3 look(p_lookX, p_lookY, p_lookZ);
			Chaos::Maths::Vector3 up(p_upX, p_upY, p_upZ);
			Chaos::Maths::Vector3 eye_look = eye - look;
			Chaos::Maths::Vector3 n(eye_look / eye_look.Length());
			Chaos::Maths::Vector3 UpXn(up.Cross(n));
			Chaos::Maths::Vector3 u(UpXn / UpXn.Length());
			Chaos::Maths::Vector3 v(n.Cross(u));
			Chaos::Maths::Matrix4 View;
			View.m00 = u.x;
			View.m01 = u.y;
			View.m02 = u.z;
			View.m03 = -eye.Dot(u);
			View.m10 = v.x;
			View.m11 = v.y;
			View.m12 = v.z;
			View.m13 = -eye.Dot(v);
			View.m20 = n.x;
			View.m21 = n.y;
			View.m22 = n.z;
			View.m23 = -eye.Dot(n);
			return View;
		}*/

		inline Matrix4 Matrix4::CreateLookAt(const float p_eyeX, const float p_eyeY, const float p_eyeZ, const float p_lookX, const float p_lookY, const float p_lookZ, const float p_upX, const float p_upY, const float p_upZ)
		{
			Chaos::Maths::Vector3 eye	= Vector3(p_eyeX, p_eyeY, p_eyeZ);
			Chaos::Maths::Vector3 zAxis = (eye - Vector3(p_lookX, p_lookY, p_lookZ)).Normalized();
			Chaos::Maths::Vector3 xAxis = Vector3(p_upX, p_upY, p_upZ).Cross(zAxis).Normalized();
			Chaos::Maths::Vector3 yAxis = zAxis.Cross(xAxis);

			Matrix4 viewMatrix(
				xAxis.x, xAxis.y, xAxis.z, -(xAxis.Dot(eye)),
				yAxis.x, yAxis.y, yAxis.z, -(yAxis.Dot(eye)),
				zAxis.x, zAxis.y, zAxis.z, -(zAxis.Dot(eye)),
				0, 0, 0, 1.f);

			return viewMatrix;
		}

		inline Matrix4 Matrix4::CreateFrustum(const float p_left, const float p_right, const float p_bottom, const float p_top, const float p_zNear, const float p_zFar)
		{
			const float maxView = 2.0f * p_zNear;
			const float width = p_right - p_left;
			const float height = p_top - p_bottom;
			const float zRange = p_zFar - p_zNear;
			Chaos::Maths::Matrix4 Frustum;
			Frustum.m00 = maxView / width;
			Frustum.m11 = maxView / height;
			Frustum.m02 = (p_right + p_left) / width;
			Frustum.m12 = (p_top + p_bottom) / height;
			Frustum.m22 = (-p_zFar - p_zNear) / zRange;
			Frustum.m32 = -1.0f;
			Frustum.m23 = (-maxView * p_zFar) / zRange;
			Frustum.m33 = 0.0f;
			return Frustum;
		}
	}
}