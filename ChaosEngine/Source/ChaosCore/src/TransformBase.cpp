#include "ChaosCore/TransformBase.h"

void Chaos::ATransform::Scale(float p_scale)
{
	m_scale += Maths::Vector3(p_scale, p_scale, p_scale);
}

void Chaos::ATransform::Scale(float x, float y, float z)
{
	m_scale += Maths::Vector3(x, y, z);
}

void Chaos::ATransform::Scale(const Maths::Vector3& p_scale)
{
	m_scale += p_scale;
}

void Chaos::ATransform::SetScale(float x, float y, float z)
{
	m_scale = Maths::Vector3(x, y, z);
}

void Chaos::ATransform::SetScale(const Maths::Vector3& p_scale)
{
	m_scale = p_scale;
}

const Chaos::Maths::Vector3& Chaos::ATransform::GetScale() const
{
	return m_scale;
}

Chaos::Maths::Matrix4 Chaos::ATransform::GetScaling() const
{
	return Maths::Matrix4::Scale(m_scale);
}
