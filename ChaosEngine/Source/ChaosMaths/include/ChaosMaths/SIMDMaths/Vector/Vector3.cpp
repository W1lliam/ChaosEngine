#include "Vector3.h"
#include <string>

namespace Chaos
{
	namespace Maths
	{
        const Vector3 Vector3::Zero     = Vector3(0.0f, 0.0f, 0.0f);
        const Vector3 Vector3::Up       = Vector3(0.0f, 1.0f, 0.0f);
        const Vector3 Vector3::Front    = Vector3(0.0f, 0.0f, 1.0f);
        const Vector3 Vector3::Right    = Vector3(1.0f, 0.0f, 0.0f);

		Vector3::Vector3()
		{
			simdVector = _mm_set_ps1(0.f);
		}
		Vector3::Vector3(float p_x, float p_y, float p_z)
		{
			simdVector = _mm_setr_ps(p_x, p_y, p_z, 0);
		}
		Vector3::Vector3(__m128 p_simdVector)
		{
			simdVector = p_simdVector;
		}
		Vector3 Vector3::operator+(const Vector3& p_other) const
		{
			__m128 toReturn = _mm_add_ps(simdVector, p_other.simdVector);
			return Vector3(toReturn);
		}
		Vector3 Vector3::operator-(const Vector3& p_other) const
		{
			__m128 toReturn = _mm_sub_ps(simdVector, p_other.simdVector);
			return Vector3(toReturn);
		}
		float Vector3::operator*(const Vector3& p_other) const
		{
			return _mm_cvtss_f32(_mm_dp_ps(simdVector, p_other.simdVector, 0xff));
		}
		Vector3 Vector3::operator*(float p_toMultiply) const
		{
			__m128 toReturn = _mm_mul_ps(simdVector, _mm_set_ps1(p_toMultiply));
			return Vector3(toReturn);

		}
		Vector3 Vector3::operator/(float p_toDivide) const
		{
			__m128 toReturn = _mm_div_ps(simdVector, _mm_set_ps1(p_toDivide));
			return Vector3(toReturn);
		}

		Vector3 Vector3::operator+=(const Vector3& p_other)
		{
			simdVector = _mm_add_ps(simdVector, p_other.simdVector);
			return *this;
		}
		Vector3 Vector3::operator-=(const Vector3& p_other)
		{
			simdVector = _mm_sub_ps(simdVector, p_other.simdVector);
			return *this;
		}
		Vector3 Vector3::operator/=(float p_toDivide)
		{
			simdVector = _mm_div_ps(simdVector, _mm_set_ps1(p_toDivide));
			return *this;
		}

		Vector3 Vector3::operator*=(float p_toMultiply)
		{
			simdVector = _mm_mul_ps(simdVector, _mm_set_ps1(p_toMultiply));
			return *this;
		}
		bool Vector3::operator==(const Vector3& p_toCompare) const
		{
			__m128	line = _mm_cmpeq_ps(simdVector, p_toCompare.simdVector);
			int	lineMask = _mm_movemask_ps(line);
			bool flag = true;
			if (lineMask != 15)
			{
				flag = false;
			}
			return flag;
		}
		bool Vector3::operator!=(const Vector3& p_toCompare) const
		{
			return !(*this == p_toCompare);
		}
		float& Vector3::operator[](const uint8_t p_index)
		{
			return p_index == 0 ? this->x : (p_index == 1 ? this->y : this->z);
		}
		const float& Vector3::operator[](const uint8_t p_index) const
		{
			return p_index == 0 ? this->x : (p_index == 1 ? this->y : this->z);
		}
		Vector3 Vector3::Cross(const Vector3& p_other) const
		{
			return _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(simdVector, simdVector, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(p_other.simdVector, p_other.simdVector, _MM_SHUFFLE(3, 1, 0, 2))),
				_mm_mul_ps(_mm_shuffle_ps(simdVector, simdVector, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(p_other.simdVector, p_other.simdVector, _MM_SHUFFLE(3, 0, 2, 1)))
			);
		}
		float Vector3::Dot(const Vector3& p_other) const
		{
			return *this * p_other;
		}
		float Vector3::Length() const
		{
			return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(simdVector, simdVector, 0xff)));
		}
		void Vector3::Normalize()
		{
			__m128 length = _mm_set_ps1(Length());
			simdVector = _mm_div_ps(simdVector, length);
		}

		Vector3 Vector3::Lerp(Vector3 & p_start, Vector3 & p_end, float p_alpha)
		{
			return p_start + (p_end - p_start) * p_alpha;
		}

        std::string Vector3::ToString()
        {
            return "{ " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + " }";
        }

		std::ostream& operator<<(std::ostream& p_stream, const Vector3& p_vector)
		{
			p_stream << "{ " << p_vector.x << ", "
				<< p_vector.y << ", "
				<< p_vector.z << " }";
			return p_stream;
		}

		Vector3 Vector3::Normalized() const
		{
			__m128 length = _mm_set_ps1(Length());
			length = _mm_div_ps(simdVector, length);
			return Vector3(length);
		}
	}
}