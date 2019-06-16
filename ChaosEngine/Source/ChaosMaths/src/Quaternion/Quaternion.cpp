#include "ChaosMaths/SIMDMaths/Quaternion/Quaternion.h"
#include "ChaosMaths/SIMDMaths/Matrix/Matrix3x3.h"

Chaos::Maths::Quaternion::Quaternion()
{
	m_element = _mm_setr_ps(0, 0, 0, 1.0f);
}

Chaos::Maths::Quaternion::Quaternion(float p_x, float p_y, float p_z, float p_w)
{
	m_element = _mm_setr_ps(p_x, p_y, p_z, p_w);
}

Chaos::Maths::Quaternion::Quaternion(Vector4& p_quat)
{
	m_element = p_quat.simdVector;
}

Chaos::Maths::Quaternion::Quaternion(const Quaternion& p_other)
{
	m_element = p_other.GetElement();
}

Chaos::Maths::Quaternion::Quaternion(__m128& p_quaternion)
{
	m_element = p_quaternion;
}

Chaos::Maths::Quaternion::Quaternion(const Quaternion&& p_other) noexcept
{
	m_element = p_other.GetElement();
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::operator=(const Quaternion& p_other)
{
	m_element = p_other.GetElement();
	return *this;
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::operator=(Quaternion&& p_other) noexcept
{
	m_element = p_other.GetElement();
	return *this;
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::Identity()
{
	return(Quaternion(0.f, 0.f, 0.f, 1.0f));
}

Chaos::Maths::Quaternion::Quaternion(const Matrix4& p_matrix)
{
	__m128 halfSquare = _mm_setr_ps(0.5f, 0.5f, 0.5f, 0.5f);
	const float trace = p_matrix.elements[0] + p_matrix.elements[5] + p_matrix.elements[10];
	__m128 Square = _mm_sqrt_ps(_mm_set1_ps(trace + 1.f));
	__m128 InvSquare;
	if (trace > 0.0f)
	{		
		InvSquare = _mm_rsqrt_ps(_mm_set_ps(trace + 1.f, trace + 1.f, trace + 1.f, 1));
		halfSquare = _mm_mul_ps(halfSquare, InvSquare);
		__m128 Quat = _mm_setr_ps(p_matrix.elements[6] - p_matrix.elements[9], p_matrix.elements[8] - p_matrix.elements[2], p_matrix.elements[1] - p_matrix.elements[4], _mm_cvtss_f32(Square));
		m_element = _mm_mul_ps(Quat, halfSquare);
	}
	else
	{
		int8_t i = 0;
		p_matrix.elements[5] > p_matrix.elements[0] ? i = 1 : p_matrix.elements[10] > p_matrix.elements[0] || p_matrix.elements[10] > p_matrix.elements[5] ? i = 2 : i = 0;
		__m128 Quat;
		//if i is 0, j is 1 and k is 2
		if (i == 0)
		{
			InvSquare = _mm_rsqrt_ps(_mm_set_ps(1.f, trace + 1.f, trace + 1.f, trace + 1.f));
			halfSquare = _mm_mul_ps(halfSquare, InvSquare);
			Quat = _mm_setr_ps(_mm_cvtss_f32(Square), p_matrix.elements[1] + p_matrix.elements[4], p_matrix.elements[2] + p_matrix.elements[8], p_matrix.elements[6] - p_matrix.elements[9]);
			m_element = _mm_mul_ps(Quat , halfSquare);
		}
		//if i is 1, j is 2 and k is 0
		else if (i == 1)
		{
			InvSquare = _mm_rsqrt_ps(_mm_set_ps(trace + 1.f, 1.f, trace + 1.f, trace + 1.f));
			halfSquare = _mm_mul_ps(halfSquare, InvSquare);
			Quat = _mm_setr_ps(p_matrix.elements[4] + p_matrix.elements[1], _mm_cvtss_f32(Square), p_matrix.elements[6] + p_matrix.elements[9], p_matrix.elements[8] - p_matrix.elements[2]);
			m_element = _mm_mul_ps(Quat, halfSquare);
		}
		//if i is 2, j is 0 and k is 1
		else
		{
			InvSquare = _mm_rsqrt_ps(_mm_set_ps(trace + 1.f, trace + 1.f, 1.f, trace + 1.f));
			halfSquare = _mm_mul_ps(halfSquare, InvSquare);
			Quat = _mm_setr_ps(p_matrix.elements[8] + p_matrix.elements[2], p_matrix.elements[9] + p_matrix.elements[6], _mm_cvtss_f32(Square), p_matrix.elements[1] - p_matrix.elements[4]);
			m_element = _mm_mul_ps(Quat, halfSquare);
		}
	}
}

void Chaos::Maths::Quaternion::MakeFromEulerAngles(float p_xAngle, float p_yAngle, float p_zAngle, int p_xOrder,
	int p_yOrder, int p_zOrder)
{	
	p_xAngle = p_xAngle * DEG_TO_RAD;
	p_yAngle = p_yAngle * DEG_TO_RAD;
	p_zAngle = p_zAngle * DEG_TO_RAD;
	p_xOrder == 1 ? (p_yOrder == 2 ? MakeFromEulerXYZ(p_xAngle, p_yAngle, p_zAngle) : MakeFromEulerXZY(p_xAngle, p_yAngle, p_zAngle))
				  : (p_yOrder == 1 ? (p_xOrder == 2 ? MakeFromEulerYXZ(p_xAngle, p_yAngle, p_zAngle) : MakeFromEulerYZX(p_xAngle, p_yAngle, p_zAngle))
				  : (p_xOrder == 2 ? MakeFromEulerZXY(p_xAngle, p_yAngle, p_zAngle) : MakeFromEulerZYX(p_xAngle, p_yAngle, p_zAngle)));
	/*
	if(p_xOrder == 1)
	{
		if (p_yOrder == 2)
			MakeFromEulerXYZ(p_xAngle, p_yAngle, p_zAngle);
		else
			MakeFromEulerXZY(p_xAngle, p_yAngle, p_zAngle);
	}
	else if(p_yOrder == 1)
	{
		if (p_xOrder == 2)
			MakeFromEulerYXZ(p_xAngle, p_yAngle, p_zAngle);
		else
			MakeFromEulerYZX(p_xAngle, p_yAngle, p_zAngle);
	}
	else
	{
		if (p_xOrder == 2)
			MakeFromEulerZXY(p_xAngle, p_yAngle, p_zAngle);
		else
			MakeFromEulerZYX(p_xAngle, p_yAngle, p_zAngle);
	}*/
}

void Chaos::Maths::Quaternion::MakeFromEulerAngles(Vector3 p_angles, int p_xOrder, int p_yOrder, int p_zOrder)
{
	p_angles.x = p_angles.x * DEG_TO_RAD;
	p_angles.y = p_angles.y * DEG_TO_RAD;
	p_angles.z = p_angles.z * DEG_TO_RAD;
	p_xOrder == 1 ? (p_yOrder == 2 ? MakeFromEulerXYZ(p_angles.x, p_angles.y, p_angles.z) : MakeFromEulerXZY(p_angles.x, p_angles.y, p_angles.z))
				  : (p_yOrder == 1 ? (p_xOrder == 2 ? MakeFromEulerYXZ(p_angles.x, p_angles.y, p_angles.z) : MakeFromEulerYZX(p_angles.x, p_angles.y, p_angles.z))
				  : (p_xOrder == 2 ? MakeFromEulerZXY(p_angles.x, p_angles.y, p_angles.z) : MakeFromEulerZYX(p_angles.x, p_angles.y, p_angles.z)));
}

void Chaos::Maths::Quaternion::MakeFromEulerXYZ(float p_xAngle, float p_yAngle, float p_zAngle)
{
	__m128 angles = _mm_mul_ps(_mm_setr_ps(p_xAngle, p_yAngle, p_zAngle, 0), _mm_set_ps1(0.5f));
	__m128 cosAngles = VecCos(angles); //cp cr cy in order
	__m128 sinAngles = VecSin(angles); //sp sr sy in order
	//p = x,  r = y, y = z

	__m128 LeftSide1 = _mm_mul_ps(_mm_setr_ps(-1.f, 1.f, -1.f, 1.f), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(0, 0, 0, 0))); //-sp sp -sp sp
	__m128 LeftSide2 = _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 2, 1, 2)), _mm_shuffle_ps(cosAngles, sinAngles, Shuffle(3, 1, 2, 1))); //1 cr sy sr
	__m128 LeftSide3 = _mm_add_ps(_mm_setr_ps(0, 1, 1, 1), _mm_mul_ps(_mm_shuffle_ps(sinAngles, sinAngles, Shuffle(2, 3, 3, 3)), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(1, 3, 3, 3)))); //srsy
	__m128 LeftSide = _mm_mul_ps(LeftSide3, _mm_mul_ps(LeftSide1, LeftSide2));

	__m128 RightSide1 = _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(2, 0, 2, 1));
	__m128 RightSide2 = _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(1, 3, 0, 0)), _mm_shuffle_ps(cosAngles, sinAngles, Shuffle(0, 3, 1, 2)));
	__m128 RightSide3 = _mm_add_ps(_mm_setr_ps(1, 0, 1, 1), _mm_mul_ps(_mm_shuffle_ps(sinAngles, sinAngles, Shuffle(3, 1, 3, 3)), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(3, 2, 3, 3)))); //srsy
	__m128 RightSide = _mm_mul_ps(RightSide1, _mm_mul_ps(RightSide2, RightSide3));

	m_element = _mm_add_ps(LeftSide, RightSide);
	m_element = _mm_shuffle_ps(m_element, m_element, Shuffle(1, 2, 3, 0));
	this->Normalize();
}

void Chaos::Maths::Quaternion::MakeFromEulerXZY(float p_xAngle, float p_yAngle, float p_zAngle)
{
	__m128 angles = _mm_mul_ps(_mm_setr_ps(p_xAngle, p_yAngle, p_zAngle, 0), _mm_set_ps1(0.5f));
	__m128 cosAngles = VecCos(angles); //cp cr cy in order
	__m128 sinAngles = VecSin(angles); //sp sr sy in order

	__m128 LeftSide1 = _mm_mul_ps(_mm_setr_ps(1.f, 1.f, -1.f, 1.f), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(0, 0, 0, 0))); //sp sp -sp sp
	__m128 LeftSide2 = _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 1, 1, 2)), _mm_shuffle_ps(cosAngles, sinAngles, Shuffle(3, 2, 2, 1))); //1 cr sy sr
	__m128 LeftSide3 = _mm_add_ps(_mm_setr_ps(0, 1, 1, 1), _mm_mul_ps(_mm_shuffle_ps(sinAngles, sinAngles, Shuffle(2, 3, 3, 3)), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(1, 3, 3, 3)))); //srsy
	__m128 LeftSide = _mm_mul_ps(LeftSide3, _mm_mul_ps(LeftSide1, LeftSide2));

	__m128 RightSide1 = _mm_mul_ps(_mm_setr_ps(1.f, -1.f, 1.f, 1.f), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(1, 0, 2, 1)));
	__m128 RightSide2 = _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(2, 3, 0, 0)), _mm_shuffle_ps(cosAngles, sinAngles, Shuffle(0, 3, 1, 2)));
	__m128 RightSide3 = _mm_add_ps(_mm_setr_ps(1, 0, 1, 1), _mm_mul_ps(_mm_shuffle_ps(sinAngles, sinAngles, Shuffle(3, 2, 3, 3)), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(3, 1, 3, 3)))); //srsy
	__m128 RightSide = _mm_mul_ps(RightSide1, _mm_mul_ps(RightSide2, RightSide3));

	m_element = _mm_add_ps(LeftSide, RightSide);//wxyz->xyzw
	m_element = _mm_shuffle_ps(m_element, m_element, Shuffle(1, 2, 3, 0));
	this->Normalize();
}

void Chaos::Maths::Quaternion::MakeFromEulerYXZ(float p_xAngle, float p_yAngle, float p_zAngle)
{
	__m128 angles = _mm_mul_ps(_mm_setr_ps(p_xAngle, p_yAngle, p_zAngle, 0), _mm_set_ps1(0.5f));
	__m128 cosAngles = VecCos(angles); //cp cr cy in order
	__m128 sinAngles = VecSin(angles); //sp sr sy in order

	__m128 LeftSide1 = _mm_mul_ps(_mm_setr_ps(1.f, 1.f, 1.f, -1.f), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(1, 1, 1, 1))); //sp sp sp -sp
	__m128 LeftSide2 = _mm_mul_ps(_mm_add_ps(_mm_setr_ps(0, 0, 1, 0), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(0, 2, 3, 0))), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 0, 2, 2))); //1 cr sy sr
	__m128 LeftSide3 = _mm_add_ps(_mm_setr_ps(0, 1, 0, 0), _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 3, 0, 3)), _mm_shuffle_ps(sinAngles, cosAngles, Shuffle(2, 3, 3, 3))));
	__m128 LeftSide = _mm_mul_ps(LeftSide3, _mm_mul_ps(LeftSide1, LeftSide2));

	__m128 RightSide1 = _mm_mul_ps(_mm_setr_ps(1.f, 1.f, -1.f, 1.f), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(2, 2, 1, 0)));
	__m128 RightSide2 = _mm_mul_ps(_mm_add_ps(_mm_setr_ps(1, 0, 0, 0), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(3, 0, 0, 2))), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(0, 1, 3, 1))); //1 cr sy sr
	__m128 RightSide3 = _mm_add_ps(_mm_setr_ps(0, 0, 0, 1), _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(1, 3, 3, 3)), _mm_shuffle_ps(cosAngles, sinAngles, Shuffle(3, 3, 2, 3))));
	__m128 RightSide = _mm_mul_ps(RightSide1, _mm_mul_ps(RightSide2, RightSide3));

	m_element = _mm_add_ps(LeftSide, RightSide);
	m_element = _mm_shuffle_ps(m_element, m_element, Shuffle(1, 2, 3, 0));
	this->Normalize();
}

void Chaos::Maths::Quaternion::MakeFromEulerYZX(float p_xAngle, float p_yAngle, float p_zAngle)
{
	__m128 angles = _mm_mul_ps(_mm_setr_ps(p_xAngle, p_yAngle, p_zAngle, 0), _mm_set_ps1(0.5f));
	__m128 cosAngles = VecCos(angles); //cp cr cy in order
	__m128 sinAngles = VecSin(angles); //sp sr sy in order

	__m128 LeftSide1 = _mm_mul_ps(_mm_setr_ps(-1.f, 1.f, 1.f, -1.f), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(1, 1, 1, 1))); //sp sp sp -sp
	__m128 LeftSide2 = _mm_mul_ps(_mm_add_ps(_mm_setr_ps(0, 0, 1, 0), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(2, 2, 3, 0))), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 0, 0, 2))); //1 cr sy sr
	__m128 LeftSide3 = _mm_add_ps(_mm_setr_ps(0, 1, 0, 0), _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 3, 2, 3)), _mm_shuffle_ps(sinAngles, cosAngles, Shuffle(0, 3, 3, 3))));
	__m128 LeftSide = _mm_mul_ps(LeftSide3, _mm_mul_ps(LeftSide1, LeftSide2));

	__m128 RightSide1 = _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(0, 2, 1, 0));
	__m128 RightSide2 = _mm_mul_ps(_mm_add_ps(_mm_setr_ps(1, 0, 0, 0), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(3, 0, 2, 2))), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(2, 1, 3, 1))); //1 cr sy sr
	__m128 RightSide3 = _mm_add_ps(_mm_setr_ps(0, 0, 0, 1), _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(1, 3, 3, 3)), _mm_shuffle_ps(cosAngles, sinAngles, Shuffle(3, 3, 0, 3))));
	__m128 RightSide = _mm_mul_ps(RightSide1, _mm_mul_ps(RightSide2, RightSide3));

	m_element = _mm_add_ps(LeftSide, RightSide);
	m_element = _mm_shuffle_ps(m_element, m_element, Shuffle(1, 2, 3, 0));
	this->Normalize();
}

void Chaos::Maths::Quaternion::MakeFromEulerZXY(float p_xAngle, float p_yAngle, float p_zAngle)
{
	__m128 angles = _mm_mul_ps(_mm_setr_ps(p_xAngle, p_yAngle, p_zAngle, 0), _mm_set_ps1(0.5f));
	__m128 cosAngles = VecCos(angles); //cp cr cy in order
	__m128 sinAngles = VecSin(angles); //sp sr sy in order

	__m128 LeftSide1 = _mm_mul_ps(_mm_setr_ps(-1.f, -1.f, 1.f, 1.f), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(2, 2, 2, 2))); //sy sy sy -sy
	__m128 LeftSide2 = _mm_mul_ps(_mm_add_ps(_mm_setr_ps(0, 0, 0, 1), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(0, 2, 0, 3))), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 0, 1, 1))); //1 cr sy sr
	__m128 LeftSide3 = _mm_add_ps(_mm_setr_ps(0, 1, 0, 0), _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 3, 3, 0)), _mm_shuffle_ps(sinAngles, cosAngles, Shuffle(1, 3, 3, 3))));
	__m128 LeftSide = _mm_mul_ps(LeftSide3, _mm_mul_ps(LeftSide1, LeftSide2));

	__m128 RightSide1 = _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(1, 1, 0, 2));
	__m128 RightSide2 = _mm_mul_ps(_mm_add_ps(_mm_setr_ps(1, 0, 0, 0), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(3, 0, 1, 0))), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(1, 0, 0, 3))); //1 cr sy sr
	__m128 RightSide3 = _mm_add_ps(_mm_setr_ps(0, 0, 1, 0), _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(2, 3, 3, 3)), _mm_shuffle_ps(cosAngles, sinAngles, Shuffle(3, 3, 3, 1))));
	__m128 RightSide = _mm_mul_ps(RightSide1, _mm_mul_ps(RightSide2, RightSide3));

	m_element = _mm_add_ps(LeftSide, RightSide);
	m_element = _mm_shuffle_ps(m_element, m_element, Shuffle(1, 2, 3, 0));
	this->Normalize();
}

void Chaos::Maths::Quaternion::MakeFromEulerZYX(float p_xAngle, float p_yAngle, float p_zAngle)
{
	__m128 angles = _mm_mul_ps(_mm_setr_ps(p_xAngle, p_yAngle, p_zAngle, 0), _mm_set_ps1(0.5f));
	__m128 cosAngles = VecCos(angles); //cp cr cy in order
	__m128 sinAngles = VecSin(angles); //sp sr sy in order

	__m128 LeftSide1 = _mm_mul_ps(_mm_setr_ps(1.f, -1.f, 1.f, 1.f), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(2, 2, 2, 2))); //sp sp sp -sp
	__m128 LeftSide2 = _mm_mul_ps(_mm_add_ps(_mm_setr_ps(0, 0, 0, 1), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(1, 1, 0, 3))), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 0, 1, 0))); //1 cr sy sr
	__m128 LeftSide3 = _mm_add_ps(_mm_setr_ps(0, 1, 0, 0), _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(3, 3, 3, 1)), _mm_shuffle_ps(sinAngles, cosAngles, Shuffle(0, 3, 3, 3))));
	__m128 LeftSide = _mm_mul_ps(LeftSide3, _mm_mul_ps(LeftSide1, LeftSide2));

	__m128 RightSide1 = _mm_mul_ps(_mm_setr_ps(1.f, 1.f, 1.f, -1.f), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(0, 1, 0, 2)));
	__m128 RightSide2 = _mm_mul_ps(_mm_add_ps(_mm_setr_ps(1, 0, 0, 0), _mm_shuffle_ps(sinAngles, sinAngles, Shuffle(3, 0, 1, 1))), _mm_shuffle_ps(cosAngles, cosAngles, Shuffle(1, 2, 2, 3))); //1 cr sy sr
	__m128 RightSide3 = _mm_add_ps(_mm_setr_ps(0, 0, 1, 0), _mm_mul_ps(_mm_shuffle_ps(cosAngles, cosAngles, Shuffle(2, 3, 3, 3)), _mm_shuffle_ps(cosAngles, sinAngles, Shuffle(3, 3, 3, 0))));
	__m128 RightSide = _mm_mul_ps(RightSide1, _mm_mul_ps(RightSide2, RightSide3));

	m_element = _mm_add_ps(LeftSide, RightSide);
	m_element = _mm_shuffle_ps(m_element, m_element, Shuffle(1, 2, 3, 0));
	this->Normalize();
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::operator+(const Quaternion& p_otherQuat) const
{
	__m128 total = _mm_add_ps(this->m_element, p_otherQuat.GetElement());
	return Quaternion(total);
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::operator+=(const Quaternion& p_otherQuat)
{
	m_element = _mm_add_ps(this->m_element, p_otherQuat.GetElement());
	return *this;
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::operator-(const Quaternion& p_otherQuat) const
{
	__m128 total = _mm_sub_ps(this->m_element, p_otherQuat.GetElement());
	return Quaternion(total);
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::operator-=(const Quaternion& p_otherQuat)
{
	m_element = _mm_sub_ps(this->m_element, p_otherQuat.GetElement());
	return *this;
}

float Chaos::Maths::Quaternion::DotProduct(const Quaternion& p_otherQuat) const
{
	return _mm_cvtss_f32(_mm_dp_ps(m_element, p_otherQuat.GetElement(), 0xff));
}

float Chaos::Maths::Quaternion::DotProduct(const Quaternion& p_left, const Quaternion& p_right)
{
	return _mm_cvtss_f32(_mm_dp_ps(p_left.m_element, p_right.GetElement(), 0xff));
}

float Chaos::Maths::Quaternion::operator|(const Quaternion& p_otherQuat) const
{
	return DotProduct(p_otherQuat);
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::operator*=(const float p_scale)
{
	m_element = _mm_mul_ps(m_element, _mm_set_ps1(p_scale));
	return *this;
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::operator*(const float p_scale) const
{
	__m128 total = _mm_mul_ps(m_element, _mm_set_ps1(p_scale));
	return Quaternion(total);
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::operator*(const Quaternion& p_otherQuat) const
{
	__m128 total = _mm_mul_ps(_mm_shuffle_ps(m_element, m_element, Shuffle(3,3,3,3)), p_otherQuat.GetElement());
	total = _mm_add_ps(total, _mm_mul_ps(_mm_mul_ps(_mm_shuffle_ps(m_element, m_element, Shuffle(0,0,0,0)), _mm_shuffle_ps(p_otherQuat.GetElement(), p_otherQuat.GetElement(), Shuffle(3, 2, 1, 0))), _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f)));
	total = _mm_add_ps(total, _mm_mul_ps(_mm_mul_ps(_mm_shuffle_ps(m_element, m_element, Shuffle(1,1,1,1)), _mm_shuffle_ps(p_otherQuat.GetElement(), p_otherQuat.GetElement(), Shuffle(2, 3, 0, 1))), _mm_setr_ps(1.0f, 1.0f, -1.0f, -1.0f)));
	total = _mm_add_ps(total, _mm_mul_ps(_mm_mul_ps(_mm_shuffle_ps(m_element, m_element, Shuffle(2,2,2,2)), _mm_shuffle_ps(p_otherQuat.GetElement(), p_otherQuat.GetElement(), Shuffle(1, 0, 3, 2))), _mm_setr_ps(-1.0f, 1.0f, 1.0f, -1.0f)));

	return Quaternion(total);
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::operator*=(const Quaternion& p_otherQuat)
{
	const Quaternion mult(*this * p_otherQuat);
	*this = mult;
	return *this;
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::FromMatrix4(const Matrix4 & t)
{
	float T = 1 + t.m00 + t.m11 + t.m22;

	float S, X, Y, Z, W;

	if (T > 0.00000001f) // to avoid large distortions!
	{
		S = sqrt(T) * 2.f;
		X = (t.m12 - t.m21) / S;
		Y = (t.m20 - t.m02) / S;
		Z = (t.m01 - t.m10) / S;
		W = 0.25f * S;
	}
	else
	{
		if (t.m00 > t.m11 && t.m00 > t.m22)
		{
			// Column 0 :
			S = sqrt(1.0f + t.m00 - t.m11 - t.m22) * 2.f;
			X = 0.25f * S;
			Y = (t.m01 + t.m10) / S;
			Z = (t.m20 + t.m02) / S;
			W = (t.m12 - t.m21) / S;
		}
		else if (t.m11 > t.m22)
		{
			// Column 1 :
			S = sqrt(1.0f + t.m11 - t.m00 - t.m22) * 2.f;
			X = (t.m01 + t.m10) / S;
			Y = 0.25f * S;
			Z = (t.m12 + t.m21) / S;
			W = (t.m20 - t.m02) / S;
		}
		else
		{   // Column 2 :
			S = sqrt(1.0f + t.m22 - t.m00 - t.m11) * 2.f;
			X = (t.m20 + t.m02) / S;
			Y = (t.m12 + t.m21) / S;
			Z = 0.25f * S;
			W = (t.m01 - t.m10) / S;
		}
	}
	return Quaternion(-X, -Y, -Z, W);
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::operator/=(const float p_scale)
{
	m_element = _mm_div_ps(m_element, _mm_set_ps1(p_scale));
	return *this;
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::operator/(const float p_scale) const
{
	__m128 total = _mm_div_ps(m_element, _mm_set_ps1(p_scale));
	return Quaternion(total);
}

__m128& Chaos::Maths::Quaternion::GetElement()
{
	return m_element;
}

__m128 Chaos::Maths::Quaternion::GetElement() const
{
	return m_element;
}

float& Chaos::Maths::Quaternion::GetRealValue()
{
	return m_w;
}

float& Chaos::Maths::Quaternion::GetX()
{
	return m_x;
}

float& Chaos::Maths::Quaternion::GetY()
{
	return m_y;
}

float& Chaos::Maths::Quaternion::GetZ()
{
	return m_z;
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::Normalize()
{
	const __m128 sqrtLengthVec = _mm_mul_ps(m_element, m_element);
	const __m128 addUp = _mm_hadd_ps(sqrtLengthVec, sqrtLengthVec);
	const __m128 total = _mm_hadd_ps(addUp, addUp);
	m_element = _mm_blendv_ps(_mm_div_ps(m_element, _mm_sqrt_ps(total)), _mm_setzero_ps(), _mm_cmplt_ps(total, _mm_set1_ps(1.e-8f)));
	return *this;
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::GetNormalized() const
{
	return Normalize(*this);
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::Normalize(const Quaternion& p_quaternion)
{
	const __m128 sqrtLengthVec = _mm_mul_ps(p_quaternion.GetElement(), p_quaternion.GetElement());
	const __m128 addUp = _mm_hadd_ps(sqrtLengthVec, sqrtLengthVec);
	const __m128 total = _mm_hadd_ps(addUp, addUp);
	__m128 normalized = _mm_blendv_ps(_mm_div_ps(p_quaternion.GetElement(), _mm_sqrt_ps(total)), _mm_setzero_ps(), _mm_cmplt_ps(total, _mm_set1_ps(1.e-8f)));
	return Quaternion(normalized);
}

float Chaos::Maths::Quaternion::Length() const
{
	return std::sqrtf(LengthSquare());
}

float Chaos::Maths::Quaternion::LengthSquare() const
{
	__m128 addition = _mm_mul_ps(m_element, m_element);
	__m128 shuf = _mm_movehdup_ps(addition); // 1, 1, 3, 3
	__m128 sums = _mm_add_ps(addition, shuf); // 0+1, 1+1, 2+3, 3+3
	shuf = _mm_movehl_ps(sums, shuf); // 2+3, 3+3, 3, 3
	sums = _mm_add_ps(sums, shuf); //0+1+2+3, x, x, x, 
	return _mm_cvtss_f32(sums);
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::Inverse()
{
	const float norm = LengthSquare();
	this->Conjugate();
	return *this /= norm;
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::Inverse(const Quaternion& p_quaternion)
{
	const float norm = p_quaternion.LengthSquare();
	Quaternion toReturn(Conjugate(p_quaternion));
	return toReturn /= norm;
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::Conjugate()
{
	m_element = _mm_mul_ps(m_element, _mm_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f));
	return *this;
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::Conjugate(const Quaternion& p_quaternion)
{
	__m128 total = _mm_mul_ps(p_quaternion.GetElement(), _mm_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f));
	return Quaternion(total);
}

Chaos::Maths::Quaternion& Chaos::Maths::Quaternion::Square()
{
	return *this *= *this;
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::Square(const Quaternion& p_quaternion)
{
	Quaternion result(p_quaternion);
	return result.Square();
}

Chaos::Maths::Vector3 Chaos::Maths::Quaternion::ToEulerAngles() const
{
	double test = m_x * m_y + m_z * m_w;
	double roll, pitch, yaw;
	if (test > 0.499)
	{ // singularity at north pole
		roll = 2 * std::atan2(m_x, m_w) * RAD_TO_DEG;
		pitch = PI * 0.5 * RAD_TO_DEG;
		yaw = 0;
	}
	else if (test < -0.499)
	{ // singularity at south pole
		roll = -2 * std::atan2(m_x, m_w) * RAD_TO_DEG;
		pitch = -PI * 0.5 * RAD_TO_DEG;
		yaw = 0;
	}
	else
	{
		roll = atan2(2 * m_y*m_w - 2 * m_x*m_z, 1 - 2 * m_y * m_y - 2 * m_z * m_z) * RAD_TO_DEG;
		pitch = asin(2 * m_x*m_y + 2 * m_z*m_w) * RAD_TO_DEG;
		yaw = atan2(2 * m_x*m_w - 2 * m_y*m_z, 1 - 2 * m_x * m_x - 2 * m_z * m_z) * RAD_TO_DEG;
	}
	return Maths::Vector3((float)yaw, (float)roll, (float)pitch);
}

Chaos::Maths::Vector3 Chaos::Maths::Quaternion::ToEulerAnglesUsingForward() const
{
	float pitch, yaw, roll;         // 3 angles
	bool QuaternionOffset = false;
	Matrix4 QuatAngles = this->ToMatrix4();
// find yaw (around y-axis) first
// NOTE: asin() returns -90~+90, so correct the angle range -180~+180
// using z value of forward vector
	yaw =  std::asinf(QuatAngles.m02) * RAD_TO_DEG;
	if (QuatAngles.m22 < 0)
	{
		if(yaw >= 0) yaw = 180.0f - yaw;
		else         yaw = 180.f - yaw;
	}
	else
	{
		if (yaw < 0) yaw = 360.f + yaw;
	}

	if (yaw > 90.f && yaw < 270.f)
		QuaternionOffset = true;
	// find roll (around z-axis) and pitch (around x-axis)
	// if forward vector is (1,0,0) or (-1,0,0), then m[0]=m[4]=m[9]=m[10]=0
	if (QuatAngles.m00 > -EPSILON && QuatAngles.m00 < EPSILON)
	{
		roll = 0;  //@@ assume roll=0
		pitch = std::atan2f(QuatAngles.m10, QuatAngles.m11) * RAD_TO_DEG;
	}
	else
	{
			roll = std::atan2f(-QuatAngles.m01, QuatAngles.m00) * RAD_TO_DEG;
			pitch = std::atan2f(-QuatAngles.m12, QuatAngles.m22) * RAD_TO_DEG;	
	}
	if(QuaternionOffset)
	{
		roll += 180.f;
		pitch += 180.f;
	}

	return Vector3(pitch, yaw, roll);
}

float Chaos::Maths::Quaternion::AngularDistance(const Quaternion& p_other) const
{
	return std::acosf(*this | p_other);
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::LootAt(const Maths::Vector3 p_sourcePoint,
	const Maths::Vector3 p_targetPoint, const Maths::Vector3 p_cameraForward, const Maths::Vector3 p_cameraUp)
{
	Maths::Vector3 forward = (p_targetPoint - p_sourcePoint).Normalized();
	float dot = p_cameraForward.Dot(forward);
	if(std::fabs(dot - (1.0f)) < 0.000001f)
	{
		return Quaternion(p_cameraUp.x, p_cameraUp.y, p_cameraUp.z, 3.1415926535897932f);
	}
	if(std::fabs(dot - (1.0f)) < 0.000001f)
	{
		return Quaternion();
	}
	float halfAngle = std::acos(dot) * 0.5f;
	Maths::Vector3 rotationAxis = forward.Cross(p_cameraForward).Normalized();
	float s = std::sin(halfAngle);
	return Quaternion(rotationAxis.x * s, rotationAxis.y *s, rotationAxis.z *s, std::cos(halfAngle));
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::Slerp(Quaternion& p_first, Quaternion& p_second, const float p_alpha)
{
	Quaternion First = p_first.Normalize();
	Quaternion Second = p_second.Normalize();

	float dot = First | Second;

	// If the dot product is negative, slerp won't take
	// the shorter path. Note that v1 and -v1 are equivalent when
	// the negation is applied to all four components. Fix by 
	// reversing one quaternion.
	if (dot < 0.0f) 
	{
		Second = Second * -1;
		dot = -dot;
	}
	const double DOT_THRESHOLD = 0.9995;
	if (dot > DOT_THRESHOLD) 
	{
		return Quaternion(Second + (Second - First) * p_alpha);
	}

	float angle = First.AngularDistance(Second);
	__m128 sinus = VecSin(_mm_setr_ps((1 - p_alpha) * angle, p_alpha * angle, angle, 0));
	//3//0//1
	sinus = _mm_div_ps(sinus, _mm_shuffle_ps(sinus, sinus, Shuffle(3,3,3,3)));
	__m128 total = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(sinus, sinus, Shuffle(0, 0, 0, 0)), First.GetElement()), _mm_mul_ps(_mm_shuffle_ps(sinus, sinus, Shuffle(1, 1, 1, 1)), Second.GetElement()));
	return Quaternion(total);
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::ApproximativeSlerp(Quaternion & p_first, Quaternion & p_second, const float p_alpha)
{
	Quaternion First = p_first.Normalize();
	Quaternion Second = p_second.Normalize();

	float dot = First | Second;

	// If the dot product is negative, slerp won't take
	// the shorter path. Note that v1 and -v1 are equivalent when
	// the negation is applied to all four components. Fix by 
	// reversing one quaternion.
	if (dot < 0.0f)
	{
		Second = Second * -1;
		dot = -dot;
	}
	const double DOT_THRESHOLD = 0.9995;
	if (dot > DOT_THRESHOLD)
	{
		return Quaternion(Second + (Second - First) * p_alpha);
	}
	return First * ((First.Inverse() * Second) * p_alpha);
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::NLerp(Quaternion & p_start, Quaternion & p_end, const float p_alpha)
{
	Quaternion result;
	float dot = p_start.DotProduct(p_end);
	float blendI = 1.0f - p_alpha;
	if (dot < 0.0f)
	{
		Quaternion tmpF = p_end * -blendI;
		Quaternion tmpS = p_start * blendI;
		result = tmpS + tmpF;
	}
	else
	{
		Quaternion tmpF = p_end * blendI;
		Quaternion tmpS = p_start * blendI;
		result = tmpS + tmpF;
	}
	return result.GetNormalized();
}

Chaos::Maths::Quaternion Chaos::Maths::Quaternion::InverseQuat(const Quaternion & p_toInverse)
{
	Quaternion temp(p_toInverse);
	temp = temp.Inverse();
	return temp;
}

Chaos::Maths::Vector4 Chaos::Maths::Quaternion::RotateAntiClockWise(Vector4 p_toRotate) const
{
	Vector4 quat = m_element;
	Vector4 t0 = quat.Cross(p_toRotate).simdVector;	
	Vector4 t1 = t0 + t0;			
	Vector4 t2 = quat.Cross(Vector4(t1));		
	__m128 t3 = _mm_add_ps(p_toRotate.simdVector, _mm_mul_ps(_mm_shuffle_ps(m_element, m_element, Shuffle(3,3,3,3)), t1.simdVector)); 
	__m128 total = _mm_add_ps(t2.simdVector, t3);
	return Vector4(total);
}

Chaos::Maths::Vector3 Chaos::Maths::Quaternion::RotateAntiClockWise(Vector3 p_toRotate) const
{

	Vector3 u(m_x, m_y, m_z);

	// Extract the scalar part of the quaternion
	float s = m_w;

	// Do the math
	return Vector3(u * 2.0f *  u.Dot(p_toRotate)
		+ p_toRotate * (s * s - u.Dot(u))
		+ u.Cross(p_toRotate) * 2.0f * s);
		/*
	Vector3 quat = m_element;
	Vector3 t0 = quat.Cross(p_toRotate).simdVector;
	Vector3 t1 = t0 + t0;
	Vector3 t2 = quat.Cross(Vector3(t1));
	__m128 t3 = _mm_add_ps(p_toRotate.simdVector, _mm_mul_ps(_mm_shuffle_ps(m_element, m_element, Shuffle(3, 3, 3, 3)), t1.simdVector));
	__m128 total = _mm_add_ps(t2.simdVector, t3);
	return Vector3(total);*/
}

Chaos::Maths::Vector4 Chaos::Maths::Quaternion::RotateClockWise(Vector4 p_toRotate) const
{
	Vector4 quat = m_element;
	Vector4 t0 = quat.Cross(p_toRotate).simdVector;
	Vector4 t1 = t0 + t0;
	Vector4 t2 = quat.Cross(Vector4(t1));
	__m128 t3 = _mm_sub_ps(p_toRotate.simdVector, _mm_mul_ps(_mm_shuffle_ps(m_element, m_element, Shuffle(3, 3, 3, 3)), t1.simdVector));
	__m128 total = _mm_add_ps(t2.simdVector, t3);
	return Vector4(total);
}

Chaos::Maths::Vector3 Chaos::Maths::Quaternion::RotateClockWise(Vector3 p_toRotate) const
{
	Vector3 quat = m_element;
	Vector3 t0 = quat.Cross(p_toRotate).simdVector;
	Vector3 t1 = t0 + t0;
	Vector3 t2 = quat.Cross(Vector3(t1));
	__m128 t3 = _mm_sub_ps(p_toRotate.simdVector, _mm_mul_ps(_mm_shuffle_ps(m_element, m_element, Shuffle(3, 3, 3, 3)), t1.simdVector));
	__m128 total = _mm_add_ps(t2.simdVector, t3);
	return Vector3(total);
}

void Chaos::Maths::Quaternion::ToString() const
{
	std::cout << "X: " << m_x <<"i"<< ", Y: " << m_y << "j" << ", Z: " << m_z <<"k" << ", W: " << m_w << std::endl;
}

void Chaos::Maths::Quaternion::ToString(const Quaternion& p_quaternion)
{
	p_quaternion.ToString();
}

Chaos::Maths::Matrix3 Chaos::Maths::Quaternion::ToMatrix3() const
{
	__m128 normalized = GetNormalized().GetElement();
	Matrix3 converted;
	/*


	1 - 2y2 - 2z2	|2xy - 2zw| 2xz + 2yw
	2xy + 2zw	|1 - 2x2 - 2z2|	2yz - 2xw
	2xz - 2yw		|2yz + 2xw|	1 - 2x2 - 2y2
	*/
	const __m128 squared = _mm_mul_ps(_mm_set_ps1(2.f), _mm_mul_ps(normalized, normalized)); //2x2, 2y2, 2z2, 2w2
	const __m128 crossed = _mm_mul_ps(_mm_set_ps1(2.f), _mm_mul_ps(normalized, _mm_shuffle_ps(normalized, normalized, Shuffle(1, 2, 0, 2)))); //2xy, 2yz, 2zx, 2wz  
	const __m128 crossed2 = _mm_mul_ps(_mm_set_ps1(2.f), _mm_mul_ps(_mm_shuffle_ps(normalized, normalized, Shuffle(0, 1, 2, 1)),
		_mm_shuffle_ps(normalized, normalized, Shuffle(3, 3, 0, 2)))); //2wx, 2wy, 2xz, 2yz

	converted.simdElements[0] = _mm_setr_ps(_mm_cvtss_f32(_mm_sub_ps(_mm_set_ps1(1.0f), _mm_dp_ps(squared, _mm_setr_ps(0.f, 1.0f, 1.0f, 0.f), 0xff))),
		_mm_cvtss_f32(_mm_dp_ps(crossed, _mm_setr_ps(1.f, 0.f, 0.f, -1.f), 0xff)),
		_mm_cvtss_f32(_mm_dp_ps(crossed2, _mm_setr_ps(0.f, 1.f, 1.f, 0.f), 0xff)),
		0.f);

	converted.simdElements[1] = _mm_setr_ps(_mm_cvtss_f32(_mm_dp_ps(crossed, _mm_setr_ps(1.f, 0.f, 0.f, 1.f), 0xff)),
		_mm_cvtss_f32(_mm_sub_ps(_mm_set_ps1(1.0f), _mm_dp_ps(squared, _mm_setr_ps(1.f, 0.0f, 1.0f, 0.f), 0xff))),
		_mm_cvtss_f32(_mm_dp_ps(_mm_shuffle_ps(crossed2, crossed2, Shuffle(3, 3, 0, 0)), _mm_setr_ps(1.f, 0.f, -1.f, 0.f), 0xff)),
		0.f);


	converted.simdElements[2] = _mm_setr_ps(_mm_cvtss_f32(_mm_dp_ps(_mm_shuffle_ps(crossed2, crossed2, Shuffle(2, 1, 0, 0)), _mm_setr_ps(1.f, -1.f, 0.f, 0.f), 0xff)),
		_mm_cvtss_f32(_mm_dp_ps(crossed2, _mm_setr_ps(1.f, 0.f, 0.f, 1.f), 0xff)),
		_mm_cvtss_f32(_mm_sub_ps(_mm_set_ps1(1.0f), _mm_dp_ps(squared, _mm_setr_ps(1.f, 1.0f, 0.0f, 0.f), 0xff))),
		0.f);

	return converted;
}

Chaos::Maths::Matrix4 Chaos::Maths::Quaternion::ToMatrix4() const
{
	__m128 normalized = GetNormalized().GetElement();
	Matrix4 converted;
	/*


	1 - 2y2 - 2z2	|2xy - 2zw| 2xz + 2yw
	2xy + 2zw	|1 - 2x2 - 2z2|	2yz - 2xw
	2xz - 2yw		|2yz + 2xw|	1 - 2x2 - 2y2
	*/
	const __m128 squared =  _mm_mul_ps(_mm_set_ps1(2.f), _mm_mul_ps(normalized, normalized)); //2x2, 2y2, 2z2, 2w2
	const __m128 crossed =  _mm_mul_ps(_mm_set_ps1(2.f), _mm_mul_ps(normalized, _mm_shuffle_ps(normalized, normalized, Shuffle(1, 2, 0, 2)))); //2xy, 2yz, 2zx, 2wz  
	const __m128 crossed2 = _mm_mul_ps(_mm_set_ps1(2.f), _mm_mul_ps(_mm_shuffle_ps(normalized, normalized, Shuffle(0, 1, 2, 1)),
						    _mm_shuffle_ps(normalized, normalized, Shuffle(3, 3, 0, 2)))); //2wx, 2wy, 2xz, 2yz
	
	converted.simdElements[0] = _mm_setr_ps(_mm_cvtss_f32(_mm_sub_ps(_mm_set_ps1(1.0f), _mm_dp_ps(squared, _mm_setr_ps(0.f, 1.0f, 1.0f, 0.f), 0xff))),
											_mm_cvtss_f32(_mm_dp_ps(crossed, _mm_setr_ps(1.f, 0.f, 0.f, -1.f), 0xff)),
											_mm_cvtss_f32(_mm_dp_ps(crossed2, _mm_setr_ps(0.f, 1.f, 1.f, 0.f), 0xff)), 
											0.f);

	converted.simdElements[1] = _mm_setr_ps(_mm_cvtss_f32(_mm_dp_ps(crossed, _mm_setr_ps(1.f, 0.f, 0.f, 1.f), 0xff)),
											_mm_cvtss_f32(_mm_sub_ps(_mm_set_ps1(1.0f), _mm_dp_ps(squared, _mm_setr_ps(1.f, 0.0f, 1.0f, 0.f), 0xff))),
											_mm_cvtss_f32(_mm_dp_ps(_mm_shuffle_ps(crossed2, crossed2, Shuffle(3,3,0,0)), _mm_setr_ps(1.f, 0.f, -1.f, 0.f), 0xff)), 
											0.f);

	
	converted.simdElements[2] = _mm_setr_ps(_mm_cvtss_f32(_mm_dp_ps(_mm_shuffle_ps(crossed2, crossed2, Shuffle(2, 1, 0, 0)), _mm_setr_ps(1.f, -1.f, 0.f, 0.f), 0xff)),
											_mm_cvtss_f32(_mm_dp_ps(crossed2, _mm_setr_ps(1.f, 0.f, 0.f, 1.f), 0xff)),
											_mm_cvtss_f32(_mm_sub_ps(_mm_set_ps1(1.0f), _mm_dp_ps(squared, _mm_setr_ps(1.f, 1.0f, 0.0f, 0.f), 0xff))),
											0.f);
	
	converted.simdElements[3] = _mm_set_ps(1.f, 0.f, 0.f, 0.0f);
	return converted;
}
