#include "ChaosCore/Transform.h"
using namespace Chaos;


Chaos::Transform::Transform()
{
	m_position = Maths::Vector3(0, 0, 0);
	m_rotation = Maths::Quaternion::Identity();
}

Transform::Transform(const Maths::Vector3& p_position, const Maths::Quaternion& p_quaternion) : m_position(p_position), m_rotation(p_quaternion)
{
}

Transform Transform::operator*(const Transform& p_other) const
{
	return TransformFromParent(p_other);
}

Transform& Transform::operator*=(const Transform& p_other)
{
	*this = *this * p_other;
	return *this;
}

void Chaos::Transform::Translate(const float x, const float y, const float z)
{
	m_position += Maths::Vector3(x, y, z);
	m_dirty = true;
}

void Chaos::Transform::Translate(const Maths::Vector3 & p_dir)
{
	m_position += p_dir;
	m_dirty = true;
}

void Chaos::Transform::SetPosition(const float x, const float y, const float z)
{
	m_position = Maths::Vector3(x, y, z);
	m_dirty = true;
}

void Chaos::Transform::SetPosition(const Maths::Vector3& p_pos)
{
	m_position = p_pos;
	m_dirty = true;
}

void Chaos::Transform::SetXPosition(const float p_pos)
{
	m_position.x = p_pos;
	m_dirty = true;
}

void Chaos::Transform::SetYPosition(const float p_pos)
{
	m_position.y = p_pos;
	m_dirty = true;
}

void Chaos::Transform::SetZPosition(const float p_pos)
{
	m_position.z = p_pos;
	m_dirty = true;
}

void Transform::Rotate(float x, float y, float z, int p_xOrder, int p_yOrder, int p_zOrder)
{
	Maths::Quaternion rotation; 
	rotation.MakeFromEulerAngles(x, y, z, p_xOrder, p_yOrder, p_zOrder);
	m_rotation *= rotation;
}

void Transform::Rotate(const Maths::Vector3& p_rotation, int p_xOrder, int p_yOrder, int p_zOrder)
{
	Maths::Quaternion rotation;
	rotation.MakeFromEulerAngles(p_rotation, p_xOrder, p_yOrder, p_zOrder);
	m_rotation *= rotation;
	m_dirty = true;
}

void Transform::Rotate(const Maths::Quaternion& p_rotation)
{
	m_rotation *= p_rotation;
}

void Transform::SetOrientation(const float x, const float y, const float z, int p_xOrder, int p_yOrder, int p_zOrder)
{
	m_rotation.MakeFromEulerAngles(x, y, z, p_xOrder, p_yOrder, p_zOrder);
    m_rotation.Normalize();
	m_dirty = true; 
}

void Transform::SetOrientation(const Maths::Vector3& p_rotation, int p_xOrder, int p_yOrder, int p_zOrder)
{
	m_rotation.MakeFromEulerAngles(p_rotation, p_xOrder, p_yOrder, p_zOrder);
    m_rotation.Normalize();
	m_dirty = true;
}

void Transform::SetOrientation(const float x, const float y, const float z, const float w)
{
    m_rotation = Maths::Quaternion(x,y,z,w);
    m_rotation.Normalize();
    m_dirty = true;
}

void Transform::SetOrientation(const Maths::Quaternion & p_rotation)
{
	m_rotation = p_rotation;
    m_rotation.Normalize();
    m_dirty = true;
}

void Transform::LookAt(const Maths::Vector3 & p_targetPosition)
{
}

void Transform::LookAt(const Maths::Vector3 & p_targetPosition, const Maths::Vector3 & p_up)
{
}

void Transform::LookAtFromQuaternion(const Maths::Vector3& p_targetPosition)
{
	Maths::Quaternion toRotate = Maths::Quaternion::LootAt(m_position, p_targetPosition, m_forward, m_up);
	SetOrientation(toRotate);
}

const Maths::Vector3 Transform::GetOrientation() const
{
	return m_rotation.ToEulerAngles();
}

const Maths::Quaternion& Transform::GetRotationQuaternion() const
{
	return m_rotation;
}

Maths::Vector3 Chaos::Transform::Up()
{
	return (m_rotation.RotateAntiClockWise(Maths::Vector3(0, 1, 0))).Normalized();
}

Maths::Vector3 Chaos::Transform::Right()
{
	return (m_rotation.RotateAntiClockWise(Maths::Vector3(1, 0, 0))).Normalized();
}

Maths::Vector3 Chaos::Transform::Forward()
{
	return (m_rotation.RotateAntiClockWise(Maths::Vector3(0, 0, 1))).Normalized();
}

void Chaos::Transform::SetTransform(const Maths::Matrix4& p_transform)
{
	m_position = Maths::Vector3(p_transform.m03, p_transform.m13, p_transform.m23);
	m_rotation = Maths::Quaternion(p_transform);

	m_dirty = true;
}

Maths::Matrix4 Chaos::Transform::GetTranslation()
{
	return Maths::Matrix4(1.0f, 0.f, 0.f, m_position.x,
						   0.f, 1.f, 0.f, m_position.y,
						   0.f, 0.f, 1.f, m_position.z,
						   0.f, 0.f, 0.f, 1.0f);
}

Maths::Matrix4 Chaos::Transform::GetRotation()
{
	return m_rotation.ToMatrix4();
}

Maths::Matrix4 Chaos::Transform::GetTransform()
{
	Maths::Matrix4 Model = GetRotation() * GetScaling();
	Model.m03 = m_position.x;
	Model.m13 = m_position.y;
	Model.m23 = m_position.z;
	return Model;
}

Maths::Vector3 Transform::TransformVector(const Maths::Vector3& p_toTransform)
{
	return m_rotation.RotateAntiClockWise(p_toTransform) + m_position;
}

Chaos::Transform Transform::TransformFromParent(const Transform& p_parent) const
{
	return Transform(p_parent.GetRotationQuaternion().GetNormalized().RotateAntiClockWise(m_position) + p_parent.GetPosition(), (p_parent.GetRotationQuaternion().GetNormalized() * m_rotation.GetNormalized()).Normalize());
}

Chaos::Transform Transform::InverseTransformFromParent(const Transform& p_parent) const
{
	Maths::Quaternion inversed = p_parent.GetRotationQuaternion().GetNormalized().Conjugate();
	return Transform(m_position - p_parent.GetPosition(), (m_rotation.GetNormalized() * inversed).Normalize());
}

Chaos::Transform Transform::GetInverse() const
{
	Maths::Quaternion temp = m_rotation.GetNormalized();
	return Transform(temp.RotateClockWise(m_position * -1), temp.Conjugate());
}

const Chaos::Maths::Vector3 & Chaos::Transform::GetPosition() const
{
	return m_position;
}

Chaos::Maths::Vector3 Chaos::Transform::GetPosition()
{
	return m_position;
}