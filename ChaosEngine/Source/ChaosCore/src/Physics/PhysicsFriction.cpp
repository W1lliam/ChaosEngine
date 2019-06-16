#include "ChaosCore/Physics/PhysicsFriction.h"


Chaos::PhysicsFriction::PhysicsFriction(float p_dynamic, float p_static, float p_restitution)
{
	m_frictionValues.dynamicFriction = p_dynamic;
	m_frictionValues.staticFriction = p_static;
	m_frictionValues.restitution = p_restitution;
}

Chaos::E_FrictionCombinedMode Chaos::PhysicsFriction::GetFrictionMode() const
{
	return m_frictionMode;
}

Chaos::FrictionValue Chaos::PhysicsFriction::GetFrictionValues() const
{
	return m_frictionValues;
}

float Chaos::PhysicsFriction::GetDynamicFriction() const
{
	return m_frictionValues.dynamicFriction;
}

float Chaos::PhysicsFriction::GetStaticFriction() const
{
	return m_frictionValues.staticFriction;
}

float Chaos::PhysicsFriction::GetRestitution() const
{
	return m_frictionValues.restitution;
}

Chaos::Maths::Vector3& Chaos::PhysicsFriction::GetFrictions()
{
	return m_frictionValues.FrictionValues;
}

void Chaos::PhysicsFriction::SetFictionMode(E_FrictionCombinedMode p_mode)
{
	m_frictionMode = p_mode;
}

void Chaos::PhysicsFriction::SetFrictions(float staticFriction, float dynamicFriction, float restitution)
{
	m_frictionValues.staticFriction = staticFriction;
	m_frictionValues.dynamicFriction = dynamicFriction;
	m_frictionValues.restitution = restitution;
}

void Chaos::PhysicsFriction::SetFrictionValue(FrictionValue p_values)
{
	m_frictionValues = p_values;
}

void Chaos::PhysicsFriction::SetDynamicFriction(float p_dynamicFriction)
{
	m_frictionValues.dynamicFriction = p_dynamicFriction;
}

void Chaos::PhysicsFriction::SetStaticFriction(float p_staticFriction)
{
	m_frictionValues.staticFriction = p_staticFriction;
}

void Chaos::PhysicsFriction::SetRestitution(float p_restitution)
{
	m_frictionValues.restitution = p_restitution;
}