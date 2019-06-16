#include "ChaosPhysics/Shape/Shape.h"

Chaos::Physics::Shape::Shape(physx::PxShape* p_shape)
{
	m_shape = p_shape;
}

physx::PxShape * Chaos::Physics::Shape::GetShape()
{
	return m_shape;
}

Chaos::Physics::ColliderType Chaos::Physics::Shape::GetType() const
{
	return m_type;
}

void Chaos::Physics::Shape::SetType(ColliderType p_type)
{
	m_type = p_type;
}

void Chaos::Physics::Shape::EnableTrigger() const
{
	if (!m_shape)
		return;

	m_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	m_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
}

void Chaos::Physics::Shape::DisableTrigger() const
{
	if (!m_shape)
		return;

	m_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
	m_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
}

void Chaos::Physics::Shape::SetTransform(physx::PxTransform p_transform)
{
	if (m_shape && p_transform.isValid())
	{
		m_transformReference = p_transform;
		m_shape->setLocalPose(m_transformReference);
	}
}

void Chaos::Physics::Shape::SetTrigger(bool p_isTrigger)
{
	if (p_isTrigger == m_isTrigger)
		return;
	
	m_isTrigger = p_isTrigger;
	if(p_isTrigger)
		EnableTrigger();

	else
		DisableTrigger();
}

void Chaos::Physics::Shape::SetID(uint64_t p_id)
{
	m_ID = p_id;
	m_shape->setName(std::to_string(p_id).c_str());
}

bool& Chaos::Physics::Shape::GetIsTrigger()
{
	return m_isTrigger;
}