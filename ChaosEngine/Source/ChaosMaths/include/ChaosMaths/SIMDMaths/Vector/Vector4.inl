namespace Chaos
{
	namespace Maths
	{
#pragma region Constructor
		inline Vector4::Vector4() : simdVector(_mm_setzero_ps())
		{}

		inline Vector4::Vector4(float x, float y, float z, float w) : simdVector(_mm_set_ps(w, z, y, x))
		{}

		inline Vector4::Vector4(__m128 p_simdVector) : simdVector(p_simdVector)
		{}
#pragma  endregion 
#pragma region Aligned Memory Management
		inline void* Vector4::operator new [](size_t p_size)
		{
			return AlignedMemoryAllocation(p_size, 16);
		}

		inline void Vector4::operator delete [](void* p_toDelete)
		{
			AlignedMemoryFree(p_toDelete);
		}
#pragma endregion 
#pragma region Arithmetic operators
		inline Vector4 Vector4::operator+(const Vector4& p_other) const
		{
			return _mm_add_ps(simdVector, p_other.simdVector);

		}
		inline Vector4 Vector4::operator-(const Vector4& p_other) const
		{
			return _mm_sub_ps(simdVector, p_other.simdVector);
		}

		inline float Vector4::operator*(const Vector4& p_other) const
		{
			return _mm_cvtss_f32(_mm_dp_ps(simdVector, p_other.simdVector, 0xff));
		}

		inline Vector4 Vector4::operator*(float p_toMultiply) const
		{
			return _mm_mul_ps(simdVector, _mm_set1_ps(p_toMultiply));
		}

		inline Vector4 Vector4::operator/(float p_toDivide) const
		{
			return _mm_div_ps(simdVector, _mm_set1_ps(p_toDivide));
		}
#pragma endregion 
#pragma region Arithmetic and Assignment operators
		inline Vector4 Vector4::operator+=(const Vector4& p_other)
		{
			simdVector = _mm_add_ps(simdVector, p_other.simdVector);
			return *this;
		}
		inline Vector4 Vector4::operator-=(const Vector4& p_other)
		{
			simdVector = _mm_sub_ps(simdVector, p_other.simdVector);
			return *this;
		}

		inline Vector4 Vector4::operator*=(float p_toMultiply)
		{
			simdVector = _mm_mul_ps(simdVector, _mm_set1_ps(p_toMultiply));
			return *this;
		}

		inline Vector4 Vector4::operator/=(float p_toDivide)
		{
			simdVector = _mm_div_ps(simdVector, _mm_set1_ps(p_toDivide));
			return *this;
		}
#pragma endregion
#pragma region Equality operators
		inline bool Vector4::operator==(const Vector4& p_toCompare) const
		{
			return (((_mm_movemask_ps(_mm_cmpeq_ps(simdVector, p_toCompare.simdVector))) & 0x7) == 0x7);
		}

		inline bool Vector4::operator!=(const Vector4& p_toCompare) const
		{
			return !(*this == p_toCompare);
		}
#pragma endregion 
#pragma region Array access operators
		inline const float& Vector4::operator[](const uint8_t p_index) const
		{
			return p_index == 0 ? this->x : ((p_index == 1 ? this->y : (p_index == 2 ? this->z : this->w)));
		}

		inline float& Vector4::operator[](const uint8_t p_index)
		{
			return p_index == 0 ? this->x : ((p_index == 1 ? this->y : (p_index == 2 ? this->z : this->w)));
		}

		inline Vector4 Vector4::Cross(const Vector4& p_other) const
		{
			return _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(simdVector, simdVector, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(p_other.simdVector, p_other.simdVector, _MM_SHUFFLE(3, 1, 0, 2))),
				_mm_mul_ps(_mm_shuffle_ps(simdVector, simdVector, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(p_other.simdVector, p_other.simdVector, _MM_SHUFFLE(3, 0, 2, 1)))
			);
		}

#pragma endregion 
#pragma region Mathematic Methods
		inline float Vector4::Dot(const Vector4& p_other) const
		{
			return _mm_cvtss_f32(_mm_dp_ps(simdVector, p_other.simdVector, 0xff));
		}
		inline float Vector4::Length() const
		{
			return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(simdVector, simdVector, 0x71)));
		}

		inline void Vector4::Normalize()
		{
			simdVector = _mm_div_ps(simdVector, _mm_sqrt_ps(_mm_dp_ps(simdVector, simdVector, 0xFF)));
		}

		inline Vector4 Vector4::Normalized() const
		{
			return _mm_div_ps(simdVector, _mm_sqrt_ps(_mm_dp_ps(simdVector, simdVector, 0xFF)));
		}

		inline void Vector4::Homogenize()
		{
			*this /= this->w;
		}

		inline Vector4 Vector4::Homogenized() const
		{
			return *this / this->w;
		}
#pragma endregion
#pragma region Console Display Methods
		inline std::ostream& operator<<(std::ostream & p_stream, const Vector4 & p_vector)
		{
			p_stream << "Vector4(" << p_vector.x << ", "
				<< p_vector.y << ", "
				<< p_vector.z << ", " << p_vector.w << " )";
			return p_stream;
		}
#pragma endregion
	}
}