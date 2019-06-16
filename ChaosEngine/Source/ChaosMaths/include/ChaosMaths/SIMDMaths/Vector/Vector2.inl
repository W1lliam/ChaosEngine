namespace Chaos
{
	namespace Maths
	{
		inline Vector2::Vector2()
		{
			simdVector = _mm_set_ps1(0.f);
		}
		inline Vector2::Vector2(float p_x, float p_y) : x(p_x), y(p_y)
		{
		}
		inline Vector2::Vector2(__m128 p_simdVector)
		{
			simdVector = p_simdVector;
		}
		inline float Vector2::operator*(const Vector2& p_other) const
		{
			return _mm_cvtss_f32(_mm_dp_ps(simdVector, p_other.simdVector, 0xff));
		}
		inline Vector2 Vector2::operator+(const Vector2& p_other) const
		{
			return Vector2(_mm_add_ps(simdVector, p_other.simdVector));

		}
		inline Vector2 Vector2::operator-(const Vector2& p_other) const
		{
			return Vector2(_mm_sub_ps(simdVector, p_other.simdVector));
		}
		inline Vector2 Vector2::operator*(float p_toMultiply) const
		{
			return Vector2(_mm_mul_ps(simdVector, _mm_set_ps1(p_toMultiply)));
		}
		inline Vector2 Vector2::operator/(float p_toDivide) const
		{
			return Vector2(_mm_div_ps(simdVector, _mm_set_ps1(p_toDivide)));
		}
		inline Vector2 Vector2::operator+=(const Vector2& p_other)
		{
			simdVector = _mm_add_ps(simdVector, p_other.simdVector);
			return *this;
		}
		inline Vector2 Vector2::operator-=(const Vector2& p_other)
		{
			simdVector = _mm_sub_ps(simdVector, p_other.simdVector);
			return *this;
		}
		inline Vector2 Vector2::operator/=(float p_toDivide)
		{
			simdVector = _mm_div_ps(simdVector, _mm_set_ps1(p_toDivide));
			return *this;
		}
		inline Vector2 Vector2::operator*=(float p_toMultiply)
		{
			simdVector = _mm_mul_ps(simdVector, _mm_set_ps1(p_toMultiply));
			return *this;
		}
		inline bool Vector2::operator==(const Vector2& p_toCompare) const
		{
			return x == p_toCompare.x && y == p_toCompare.y;
		}

		inline bool Vector2::operator!=(const Vector2& p_toCompare) const
		{
			return x != p_toCompare.x || y != p_toCompare.y;
		}
		inline float Vector2::Dot(const Vector2& p_other) const
		{
			return *this * p_other;
		}

		inline float Vector2::Length() const
		{
			return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(simdVector, simdVector, 0xff)));
		}
		inline void Vector2::Normalize()
		{
			*this /= Length();
		}
		inline Vector2 Vector2::Normalized() const
		{
			return *this / Length();
		}
		inline std::ostream& operator<<(std::ostream& p_stream, const Vector2& p_vector)
		{
			p_stream << "Vector2(" << p_vector.x << ", "
				<< p_vector.y << ", "
				<< " )";
			return p_stream;
		}

		inline const float& Vector2::operator[](const uint8_t p_index) const
		{
			return p_index == 0 ? this->x : this->y;
		}
		inline float& Vector2::operator[](const uint8_t p_index)
		{
			return p_index == 0 ? this->x : this->y;
		}
	}
}