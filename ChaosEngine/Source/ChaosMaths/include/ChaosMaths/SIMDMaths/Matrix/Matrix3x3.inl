#include "Matrix3x3.h"
namespace Chaos
{
	namespace Maths
	{
		inline Matrix3::Matrix3()
		{
			*this = Identity();
		}
		inline Matrix3::Matrix3(float p_00, float p_01, float p_02, float p_10, float p_11, float p_12, float p_20, float p_21, float p_22)
		{
			simdElements[0] = _mm_setr_ps(p_00, p_01, p_02, 0.f);
			simdElements[1] = _mm_setr_ps(p_10, p_11, p_12, 0.f);
			simdElements[2] = _mm_setr_ps(p_20, p_21, p_22, 0.f);
		}
		inline Matrix3::Matrix3(Vector3& p_firstRow, Vector3& p_secondRow, Vector3& p_lastRow)
		{
			simdElements[0] = p_firstRow.simdVector;	
			simdElements[1] = p_secondRow.simdVector;
			simdElements[2] = p_lastRow.simdVector;
		}
		inline Matrix3::Matrix3(__m128& p_firstRow, __m128& p_secondRow, __m128& p_lastRow)
		{
			simdElements[0] = p_firstRow;
			simdElements[1] = p_secondRow;
			simdElements[2] = p_lastRow;
		}

		inline Matrix3 Matrix3::operator+(const Matrix3& p_other) const
		{
			Matrix3 add;
			for (int i = 0; i < 3; ++i)
			{
				add.simdElements[i] = _mm_add_ps(simdElements[i], p_other.simdElements[i]);
			}
			return add;
		}
		inline Matrix3 Matrix3::operator-(const Matrix3& p_other) const
		{
			Matrix3 sub;
			for (int i = 0; i < 3; ++i)
			{
				sub.simdElements[i] = _mm_sub_ps(simdElements[i], p_other.simdElements[i]);
			}
			return sub;
		}
		inline Matrix3 Matrix3::operator*(const Matrix3& p_other) const
		{
			const __m128* vecA = p_other.simdElements;
			const __m128* vecB = simdElements;
			Matrix3 multiplied;
			// line 0
			multiplied.simdElements[0] = _mm_mul_ps(vecA[0], _mm_shuffle_ps(vecB[0], vecB[0], Shuffle(0, 0, 0, 0)));
			multiplied.simdElements[0] = _mm_add_ps(multiplied.simdElements[0], _mm_mul_ps(vecA[1], _mm_shuffle_ps(vecB[0], vecB[0], Shuffle(1, 1, 1, 1))));
			multiplied.simdElements[0] = _mm_add_ps(multiplied.simdElements[0], _mm_mul_ps(vecA[2], _mm_shuffle_ps(vecB[0], vecB[0], Shuffle(2, 2, 2, 2))));
			multiplied.simdElements[0] = _mm_mul_ps(multiplied.simdElements[0], _mm_setr_ps(1.f,1.f,1.f,0.f));

		
			// line 1
			multiplied.simdElements[1] = _mm_mul_ps(vecA[0], _mm_shuffle_ps(vecB[1], vecB[1], Shuffle(0, 0, 0, 0)));
			multiplied.simdElements[1] = _mm_add_ps(multiplied.simdElements[1], _mm_mul_ps(vecA[1], _mm_shuffle_ps(vecB[1], vecB[1], Shuffle(1, 1, 1, 1))));
			multiplied.simdElements[1] = _mm_add_ps(multiplied.simdElements[1], _mm_mul_ps(vecA[2], _mm_shuffle_ps(vecB[1], vecB[1], Shuffle(2, 2, 2, 2))));
			multiplied.simdElements[1] = _mm_mul_ps(multiplied.simdElements[1], _mm_setr_ps(1.f, 1.f, 1.f, 0.f));

			// line 2
			multiplied.simdElements[2] = _mm_mul_ps(vecA[0], _mm_shuffle_ps(vecB[2], vecB[2], Shuffle(0, 0, 0, 0)));
			multiplied.simdElements[2] = _mm_add_ps(multiplied.simdElements[2], _mm_mul_ps(vecA[1], _mm_shuffle_ps(vecB[2], vecB[2], Shuffle(1, 1, 1, 1))));
			multiplied.simdElements[2] = _mm_add_ps(multiplied.simdElements[2], _mm_mul_ps(vecA[2], _mm_shuffle_ps(vecB[2], vecB[2], Shuffle(2, 2, 2, 2))));
			multiplied.simdElements[2] = _mm_mul_ps(multiplied.simdElements[2], _mm_setr_ps(1.f, 1.f, 1.f, 0.f));

			return multiplied;
		}
		inline Matrix3 Matrix3::operator/(const Matrix3& p_other) const
		{
			
		}
		inline Matrix3 Matrix3::operator*(float p_toMultiply) const
		{
			Matrix3 mult;
			__m128 scalar = _mm_set_ps1(p_toMultiply);
			for (int i = 0; i < 3; ++i)
			{
				mult.simdElements[i] = _mm_sub_ps(simdElements[i], scalar);
			}
			return mult;
		}
		inline Matrix3 Matrix3::operator/(float p_toDivide) const
		{
			Matrix3 mult;
			__m128 scalar = _mm_set_ps1(p_toDivide);
			for (int i = 0; i < 3; ++i)
			{
				mult.simdElements[i] = _mm_div_ps(simdElements[i], scalar);
			}
			return mult;
		}
		inline Vector3 Matrix3::operator*(const Vector3& p_other) const
		{
			__m128 first = _mm_mul_ps(simdElements[0], p_other.simdVector);
			__m128 second = _mm_mul_ps(simdElements[1], p_other.simdVector);
			__m128 third = _mm_mul_ps(simdElements[2], p_other.simdVector);

			return Vector3(_mm_cvtss_f32(_mm_dp_ps(first, _mm_set_ps1(1.0f), 0xff)),
				_mm_cvtss_f32(_mm_dp_ps(second, _mm_set_ps1(1.0f), 0xff)),
				_mm_cvtss_f32(_mm_dp_ps(third, _mm_set_ps1(1.0f), 0xff)));
	
		}
		inline Matrix3 Matrix3::operator+=(const Matrix3& p_other)
		{
			for (int i = 0; i < 3; ++i)
			{
				simdElements[i] = _mm_add_ps(simdElements[i], p_other.simdElements[i]);
			}
			return *this;
		}
		inline Matrix3 Matrix3::operator-=(const Matrix3& p_other)
		{
			for (int i = 0; i < 3; ++i)
			{
				simdElements[i] = _mm_sub_ps(simdElements[i], p_other.simdElements[i]);
			}
			return *this;
		}
		inline Matrix3 Matrix3::operator*=(const Matrix3& p_other)
		{
			*this = *this * p_other;
			return *this;
		}
		inline Matrix3 Matrix3::operator/=(const Matrix3& p_other)
		{
			
		}
		inline Matrix3 Matrix3::operator/=(float p_toDivide)
		{
			__m128 scalar = _mm_set_ps1(p_toDivide);
			for (int i = 0; i < 3; ++i)
			{
				simdElements[i] = _mm_div_ps(simdElements[i],scalar);
			}
			return *this;
		}
		inline Matrix3 Matrix3::operator*=(float p_toMultiply)
		{
			__m128 scalar = _mm_set_ps1(p_toMultiply);
			for (int i = 0; i < 3; ++i)
			{
				simdElements[i] = _mm_mul_ps(simdElements[i], scalar);
			}
			return *this;
		}

		inline bool Matrix3::operator==(const Matrix3& p_other)
		{
			__m128 line;
			int lineMask;
			for (int i = 0; i < 3; ++i)
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
		inline bool Matrix3::operator!=(const Matrix3& p_other)
		{
			return !(*this == p_other);
		}
		inline bool Matrix3::IsOrthogonal() const
		{
			Matrix3 identity;
			return (identity == *this * Transpose());
		}
		inline std::string Matrix3::ToString() const
		{
			std::stringstream ss;
			ss << elements[0] << "\t" << elements[1] << "\t" << elements[2] << "\n" <<
				elements[4] << "\t" << elements[5] << "\t" << elements[6] << "\n" <<
				elements[8] << "\t" << elements[9] << "\t" << elements[10] << "\n";
			std::string result = ss.str();
			return result;
		}

		inline Matrix3 Matrix3::Identity()
		{
			return Matrix3(1.0f, 0.0f, 0.0f,
						   0.0f, 1.0f, 0.f,
						   0.0f, 0.0f, 1.0f);
		}
		inline float Matrix3::Determinant() const
		{
			//Sarrus rule is simpler to write compared to Laplace expression
			__m128 coeff = _mm_set_ps1(1.0f);
      		__m128 leftSide = _mm_dp_ps(_mm_mul_ps(_mm_mul_ps(simdElements[0], 
							  _mm_shuffle_ps(simdElements[1], simdElements[1], Shuffle(1,2,0,3))), 
							  _mm_shuffle_ps(simdElements[2], simdElements[2], Shuffle(2, 0, 1, 3))), coeff, 0xff);

			__m128 rightSide = _mm_dp_ps(_mm_mul_ps(_mm_mul_ps(simdElements[0], 
							   _mm_shuffle_ps(simdElements[1], simdElements[1], Shuffle(2, 0, 1, 3))),
							   _mm_shuffle_ps(simdElements[2], simdElements[2], Shuffle(1, 2, 0, 3))), coeff, 0xff);

			return _mm_cvtss_f32(_mm_sub_ps(leftSide, rightSide));
		}

		inline float Matrix3::SimpleDeterminant() const
		{
			Matrix3 transposed = Transpose();
			Vector3 firstLine = transposed.simdElements[0];
			Vector3 secondLine = transposed.simdElements[1];
			Vector3 thirdLine = transposed.simdElements[2];

			return firstLine.Dot((secondLine.Cross(thirdLine)));
		}

		inline Matrix3 Matrix3::Inverse() const
		{
			if (IsOrthogonal())
				return Transpose();
			float invDet =  1.f / Determinant();
			Matrix3 inverse = Transpose();
			Vector3 firstLine = inverse.simdElements[0];
			Vector3 secondLine = inverse.simdElements[1];
			Vector3 thirdLine = inverse.simdElements[2];

			inverse.simdElements[0] = secondLine.Cross(thirdLine).simdVector;
			inverse.simdElements[1] = thirdLine.Cross(firstLine).simdVector;
			inverse.simdElements[2] = firstLine.Cross(secondLine).simdVector;

			return inverse.Transpose() * invDet;
		}
		inline void Matrix3::Inversed()
		{
			if (IsOrthogonal())
				Transposed();

			float invDet = 1.f / Determinant();
			Matrix3 inverse = Transpose();
			Vector3 firstLine = inverse.simdElements[0];
			Vector3 secondLine = inverse.simdElements[1];
			Vector3 thirdLine = inverse.simdElements[2];

			inverse.simdElements[0] = secondLine.Cross(thirdLine).simdVector;
			inverse.simdElements[1] = thirdLine.Cross(firstLine).simdVector;
			inverse.simdElements[2] = firstLine.Cross(secondLine).simdVector;

			*this = (inverse.Transpose() * invDet);
		}
		inline float& Matrix3::operator()(uint8_t p_row, uint8_t p_column)
		{
			if (p_row >= 3 || p_column >= 3)
				throw std::out_of_range(
					"Invalid index : " + std::to_string(p_row) + "," + std::to_string(p_column) + " is out of range");
			return elements[4 * p_row + p_column];
		}
		inline Matrix3 Matrix3::Translation(const Vector2 & p_translation)
		{
			return Matrix3(1, 0, p_translation.x,
						   0, 1, p_translation.y,
						   0, 0, 1);
		}
		inline Matrix3 Matrix3::Rotation(const float p_angle)
		{
			float cos = std::cosf(p_angle);
			float sin = std::sinf(p_angle);
			return Matrix3(cos, -sin, 0,
						   sin, cos, 0,
						   0, 0, 1);
		}
		inline Matrix3 Matrix3::Scale(const Vector2 & p_scaling)
		{
			return Matrix3(p_scaling.x, 0, 0,
						   0, p_scaling.y, 0,
				           0, 0, 1);
		}
		inline Matrix3 Matrix3::Transpose() const
		{
			//00, 01 ,02, 0[0]
			//10, 11, 12, 0[1]
			//20, 21, 22, 0[2]
			Matrix3 transposed;
			__m128 t0 = _mm_movelh_ps(simdElements[0], simdElements[1]); // 00, 01, 10, 11
			__m128 t1 = _mm_movehl_ps(simdElements[1], simdElements[0]); // 02, 0, 12, 0

			transposed.simdElements[0] = _mm_shuffle_ps(t0, simdElements[2], Shuffle(0, 2, 0, 3)); // 00, 10, 20, 0
			transposed.simdElements[1] = _mm_shuffle_ps(t0, simdElements[2], Shuffle(1, 3, 1, 3)); // 01, 11, 21, 0
			transposed.simdElements[2] = _mm_shuffle_ps(t1, simdElements[2], Shuffle(0, 2, 2, 0)); // 02, 12, 22, 0
			return transposed;
		}
		inline void Matrix3::Transposed()
		{
			__m128 t0 = _mm_movelh_ps(simdElements[0], simdElements[1]); // 00, 01, 10, 11
			__m128 t1 = _mm_movehl_ps(simdElements[1], simdElements[0]); // 02, 0, 12, 0
			simdElements[0] = _mm_shuffle_ps(t0, simdElements[2], Shuffle(0, 2, 0, 3)); // 00, 10, 20, 0
			simdElements[1] = _mm_shuffle_ps(t0, simdElements[2], Shuffle(1, 3, 1, 3)); // 01, 11, 21, 0
			simdElements[2] = _mm_shuffle_ps(t1, simdElements[2], Shuffle(0, 2, 2, 0)); // 02, 12, 22, 0
		}
	}
}
