#include "ChaosMaths/SIMDMaths/DualQuaternion/DualQuaternion.h"

Chaos::Maths::DualQuaternion::DualQuaternion()
{
	DualQuaternion temp = Identity();
	m_dual = temp.GetTranslation();
	m_nonDual = temp.GetRotation();
}

Chaos::Maths::DualQuaternion::DualQuaternion(const Quaternion& p_nonDual, const Quaternion& p_dual)
{
	m_nonDual = p_nonDual;
	m_dual = p_dual;
}

Chaos::Maths::DualQuaternion::DualQuaternion(const __m128& p_nonDual, const __m128& p_dual)
{
	m_nonDual.GetElement() = p_nonDual;
	m_dual.GetElement() = p_dual;
}

Chaos::Maths::DualQuaternion::DualQuaternion(const Quaternion& p_rotation, const Vector4& p_translation)
{
	DualQuaternion res = FromRotationAndTranslation(p_rotation, p_translation);
	m_nonDual = res.GetRotation();
	m_dual = res.GetTranslation();
}

Chaos::Maths::DualQuaternion::DualQuaternion(const Matrix4& p_toConvert)
{
	Quaternion rotation(p_toConvert);
	Vector4 translation(p_toConvert.elements[3], p_toConvert.elements[7], p_toConvert.elements[11], 0);
	DualQuaternion res = FromRotationAndTranslation(rotation, translation);
	m_nonDual = res.GetRotation();
	m_dual = res.GetTranslation();
}

Chaos::Maths::DualQuaternion::DualQuaternion(const DualQuaternion & p_other)
{
	m_dual = p_other.GetTranslation();
	m_nonDual = p_other.GetRotation();
}

Chaos::Maths::DualQuaternion::DualQuaternion(const DualQuaternion&& p_other) noexcept
{
	m_nonDual = p_other.GetRotation();
	m_dual = p_other.GetTranslation();
}

Chaos::Maths::DualQuaternion& Chaos::Maths::DualQuaternion::operator=(const DualQuaternion& p_other)
{
	m_nonDual = p_other.GetRotation();
	m_dual = p_other.GetTranslation();
	return *this;
}

Chaos::Maths::DualQuaternion& Chaos::Maths::DualQuaternion::operator=(DualQuaternion&& p_other) noexcept
{
	m_nonDual = p_other.GetRotation();
	m_dual = p_other.GetTranslation();
	return *this;
}

Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::Identity()
{
	__m128 dual = _mm_setr_ps(0.f, 0.f, 0.f, 1.0f);
	__m128 nondual = _mm_set_ps1(0.f);
	return DualQuaternion(nondual, dual);
}

Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::FromRotationAndTranslation(const Quaternion& p_rotation,
	const Vector4& p_translation)
{
	__m128 scalar =  _mm_setr_ps(0.5f, 0.5f, 0.5f, -0.5f);
	__m128 rotation = p_rotation.GetElement();
	__m128 total = _mm_setr_ps(_mm_cvtss_f32(_mm_dp_ps(_mm_mul_ps(p_translation.simdVector, _mm_setr_ps(1.0f, 1.0f, -1.0f, 0.0f)), _mm_shuffle_ps(rotation, rotation, Shuffle(3, 2, 1, 1)), 0xff)),
							   _mm_cvtss_f32(_mm_dp_ps(_mm_mul_ps(p_translation.simdVector, _mm_setr_ps(-1.0f, 1.0f, 1.0f, 0.0f)), _mm_shuffle_ps(rotation, rotation, Shuffle(2, 3, 0, 1)), 0xff)),
							   _mm_cvtss_f32(_mm_dp_ps(_mm_mul_ps(p_translation.simdVector, _mm_setr_ps(1.0f, -1.0f, 1.0f, 0.0f)), _mm_shuffle_ps(rotation, rotation, Shuffle(1, 0, 3, 1)), 0xff)),
							   _mm_cvtss_f32(_mm_dp_ps(_mm_mul_ps(p_translation.simdVector, _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f)),  _mm_shuffle_ps(rotation, rotation, Shuffle(0, 1, 2, 3)), 0xff)));

	total = _mm_mul_ps(total, scalar);
	return DualQuaternion(p_rotation.GetElement(), total);
}

Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::operator+(const DualQuaternion& p_other) const
{
	return DualQuaternion(_mm_add_ps(m_nonDual.GetElement(), p_other.GetRotation().GetElement()), _mm_add_ps(m_dual.GetElement(), p_other.GetTranslation().GetElement()));
}

Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::operator-(const DualQuaternion& p_other) const
{
	return DualQuaternion(_mm_sub_ps(m_nonDual.GetElement(), p_other.GetRotation().GetElement()), _mm_sub_ps(m_dual.GetElement(), p_other.GetTranslation().GetElement()));
}

inline Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::operator*(const DualQuaternion & p_other) const
{
	return DualQuaternion(GetRotation() * p_other.GetRotation(),
	(GetRotation() * p_other.GetTranslation()) + (GetTranslation() * p_other.GetRotation()));
}

Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::operator*(float p_toMultiply) const
{
	__m128 scalar = _mm_set_ps1(p_toMultiply);
	return DualQuaternion(_mm_mul_ps(m_nonDual.GetElement(), scalar), _mm_mul_ps(m_dual.GetElement(), scalar));
}

Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::operator/(float p_toDivide) const
{
	__m128 scalar = _mm_set_ps1(p_toDivide);
	return DualQuaternion(_mm_mul_ps(m_nonDual.GetElement(), scalar), _mm_mul_ps(m_dual.GetElement(), scalar));
}

Chaos::Maths::DualQuaternion& Chaos::Maths::DualQuaternion::operator+=(const DualQuaternion& p_other)
{
	m_nonDual = (*this + p_other).GetRotation();
	m_dual = (*this + p_other).GetTranslation();
	return *this;
}

Chaos::Maths::DualQuaternion& Chaos::Maths::DualQuaternion::operator-=(const DualQuaternion& p_other)
{
	m_nonDual = (*this - p_other).GetRotation();
	m_dual = (*this - p_other).GetTranslation();
	return *this;
}

Chaos::Maths::DualQuaternion& Chaos::Maths::DualQuaternion::operator*=(float p_toMultiply)
{
	__m128 scalar = _mm_set_ps1(p_toMultiply);
	m_nonDual.GetElement() = _mm_mul_ps(m_nonDual.GetElement(), scalar);
	m_dual.GetElement() = _mm_mul_ps(m_dual.GetElement(), scalar);
	return *this;
}

inline Chaos::Maths::DualQuaternion& Chaos::Maths::DualQuaternion::operator*=(const DualQuaternion& p_other)
{
	DualQuaternion temp =  *this * p_other;
	this->m_nonDual = temp.GetRotation();
	this->m_dual = temp.GetTranslation();
	return *this;
}

Chaos::Maths::DualQuaternion& Chaos::Maths::DualQuaternion::operator/=(float p_toDivide)
{
	__m128 scalar = _mm_set_ps1(p_toDivide);
	m_nonDual.GetElement() = _mm_div_ps(m_nonDual.GetElement(), scalar);
	m_dual.GetElement() = _mm_div_ps(m_dual.GetElement(), scalar);
	return *this;
}

Chaos::Maths::Quaternion& Chaos::Maths::DualQuaternion::GetTranslationQuat()
{
	return m_nonDual;
}

Chaos::Maths::Quaternion& Chaos::Maths::DualQuaternion::GetRotationQuat()
{
	return m_dual;
}

Chaos::Maths::Quaternion Chaos::Maths::DualQuaternion::GetTranslation() const
{
	return m_dual;
}

Chaos::Maths::Quaternion Chaos::Maths::DualQuaternion::GetRotation() const
{
	return m_nonDual;
}

void Chaos::Maths::DualQuaternion::SetRotationQuat(const Quaternion& p_rotation)
{
	m_dual = p_rotation;
}

void Chaos::Maths::DualQuaternion::SetTranslationQuat(const Quaternion& p_translation)
{
	m_nonDual = p_translation;
}

void Chaos::Maths::DualQuaternion::Normalized()
{
	*this /= m_nonDual.Length();
}

Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::Normalize() const
{
	return *this / m_nonDual.Length();
}

Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::Conjugate() const
{
	return DualQuaternion(Quaternion::Conjugate(m_nonDual), Quaternion::Conjugate(m_dual));
}

Chaos::Maths::Vector4 Chaos::Maths::DualQuaternion::Transform(const Vector4& p_toTransform) const
{
	Quaternion nonDual = m_nonDual.GetNormalized();
	Quaternion Dual = m_dual.GetNormalized();


	Vector4 rotation = Dual.GetElement();
	Vector4 trans = nonDual.GetElement();
	__m128 scalar1 = _mm_set_ps1(rotation.w);
	__m128 scalar2 = _mm_set_ps1(trans.w);
	__m128 scalar3 = _mm_set_ps1(2.0f);
	
	Vector4 translated = 
	_mm_mul_ps(_mm_add_ps(rotation.Cross(trans).simdVector, 
	_mm_sub_ps(_mm_mul_ps(trans.simdVector, scalar1), 
	_mm_mul_ps(rotation.simdVector, scalar2))), scalar3);
	
	// Rotate
	return nonDual.RotateAntiClockWise(p_toTransform) + translated;
}

Chaos::Maths::Vector4 Chaos::Maths::DualQuaternion::Translate(const Vector4& p_toTranslate) const
{
	Quaternion translation = (m_dual * 2.0f) * Quaternion::Conjugate(m_nonDual);
	return Vector4(translation.GetElement());
}

Chaos::Maths::Vector4 Chaos::Maths::DualQuaternion::Rotate(const Vector4& p_toRotate) const
{
	Quaternion tmp = m_nonDual;
	tmp = tmp.Normalize();
	return tmp.RotateAntiClockWise(p_toRotate);
}

Chaos::Maths::DualQuaternion Chaos::Maths::DualQuaternion::Inverse() const
{
	float sqrLen0 = m_nonDual.Length();
	Vector4 realV = m_nonDual.GetElement();
	Vector4 dualV = m_dual.GetElement();
	float sqrLenE = -2.0f * (realV.Dot(dualV));
	
	if (sqrLen0 > 0.0)
	{
		Quaternion real = Quaternion::Conjugate(m_nonDual);
		Quaternion dual = Quaternion::Conjugate(m_dual);
		
		dual += (real * sqrLenE);
		return DualQuaternion(real, dual);

	}
	else
	{
		return DualQuaternion(_mm_set_ps1(0.f), _mm_set_ps1(0.f));
	}
}

Chaos::Maths::Matrix4 Chaos::Maths::DualQuaternion::To4x4Matrix()
{
	Matrix4 converted = (m_nonDual.Normalize()).ToMatrix4().Transpose();
	__m128 norm = _mm_div_ps(_mm_set_ps1(1.0f), _mm_set_ps1(m_nonDual.Length()));
	__m128 scalar = _mm_set_ps(2.f, 2.f, 2.f, 0);
	__m128 dualElement = m_dual.GetElement();
	__m128 nonDualElement = m_nonDual.GetElement();
	
	__m128 lastLine =
			_mm_setr_ps(
			_mm_cvtss_f32(_mm_dp_ps(_mm_mul_ps(_mm_setr_ps(1.0f, -1.f, 1.f, -1.f), dualElement), _mm_shuffle_ps(nonDualElement, nonDualElement, Shuffle(3, 2, 1, 0)), 0xff)),
			_mm_cvtss_f32(_mm_dp_ps(_mm_mul_ps(_mm_setr_ps(1.0f, 1.f, -1.f, -1.f), dualElement), _mm_shuffle_ps(nonDualElement, nonDualElement, Shuffle(2, 3, 0, 1)), 0xff)),
			_mm_cvtss_f32(_mm_dp_ps(_mm_mul_ps(_mm_setr_ps(-1.0f, 1.f, 1.f, -1.f), dualElement), _mm_shuffle_ps(nonDualElement, nonDualElement, Shuffle(1, 0, 3, 2)), 0xff)),
			0.f);

	lastLine = _mm_mul_ps(_mm_mul_ps(scalar, lastLine), norm);
	converted.simdElements[3] = lastLine;
	return converted.Transpose();
}
