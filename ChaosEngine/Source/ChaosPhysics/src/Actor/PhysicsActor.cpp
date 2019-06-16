#include "ChaosPhysics/Actor/PhysicsActor.h"

Chaos::Physics::Actor::Actor(ActorType p_type, physx::PxVec3 p_position, physx::PxQuat p_rotation, physx::PxVec3 p_velocity, float p_mass) : m_type(p_type)
{
    if (m_type == ActorType::Dynamic)
    {
        m_rigidActor = static_cast<physx::PxRigidActor*>(ElementCreator::Get().GetPxPhysics()->createRigidDynamic(physx::PxTransform(p_position, p_rotation)));
        physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(m_rigidActor), p_mass);
        static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setLinearVelocity(p_velocity);
    }
    else if (m_type == ActorType::Static)
    {
        m_rigidActor = static_cast<physx::PxRigidActor*>(ElementCreator::Get().GetPxPhysics()->createRigidStatic(physx::PxTransform(p_position, p_rotation)));
    }
}

Chaos::Physics::Actor::Actor(ActorType p_type, physx::PxTransform p_transform, physx::PxVec3 p_velocity, float p_mass) : m_type(p_type)
{
    if (m_type == ActorType::Dynamic)
    {
        m_rigidActor = static_cast<physx::PxRigidActor*>(ElementCreator::Get().GetPxPhysics()->createRigidDynamic(p_transform));
        physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(m_rigidActor), p_mass);
        static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setLinearVelocity(p_velocity);
    }
    else if (m_type == ActorType::Static)
    {
        m_rigidActor = static_cast<physx::PxRigidActor*>(ElementCreator::Get().GetPxPhysics()->createRigidStatic(p_transform));
    }
}

Chaos::Physics::Actor::~Actor()
{
}

void Chaos::Physics::Actor::AddShape(Shape* p_shape) const
{
	m_rigidActor->attachShape(*p_shape->GetShape());
}

void Chaos::Physics::Actor::RemoveShape(Shape* p_shape) const
{
	m_rigidActor->detachShape(*p_shape->GetShape());
}

void Chaos::Physics::Actor::SetAngularVelocity(const Maths::Vector3& p_angularVelocity)
{
    if (m_type == ActorType::Dynamic)
    {
        static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setAngularVelocity(Converter::ConvertVector3ToPxVec3(p_angularVelocity));
    }
}

void Chaos::Physics::Actor::SetVelocity(const Maths::Vector3& p_velocity) 
{ 
    if (m_type == ActorType::Dynamic)
    {
        static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setLinearVelocity(Converter::ConvertVector3ToPxVec3(p_velocity));
    }
}

void Chaos::Physics::Actor::SetMass(float p_mass)
{
    if (m_type == ActorType::Dynamic)
    {
        physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(m_rigidActor), p_mass);
    }
}

void Chaos::Physics::Actor::SetTransform(physx::PxTransform p_transform)
{
    m_rigidActor->setGlobalPose(p_transform);
}

physx::PxVec3 Chaos::Physics::Actor::GetVelocity() const
{
    if (m_type == ActorType::Dynamic)
    {
        return static_cast<physx::PxRigidDynamic*>(m_rigidActor)->getLinearVelocity();
    }
    return {};
}

float Chaos::Physics::Actor::GetMass() const 
{ 
    if (m_type == ActorType::Dynamic)
    {
        return static_cast<physx::PxRigidDynamic*>(m_rigidActor)->getMass();
    }
    return -1.0f;
}

void Chaos::Physics::Actor::SetXRotationConstraint(bool p_rotationFlag)
{
	if (m_type != ActorType::Dynamic || !m_rigidActor || m_rotationConstraints.x == p_rotationFlag)
		return;

	if (p_rotationFlag)
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);

	else
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, false);
	
	m_rotationConstraints.x = p_rotationFlag;
	
}

void Chaos::Physics::Actor::SetYRotationConstraint(bool p_rotationFlag)
{
	if (m_type != ActorType::Dynamic || !m_rigidActor || m_rotationConstraints.y == p_rotationFlag)
		return;

	if (p_rotationFlag)
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	
	else
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, false);

	m_rotationConstraints.y = p_rotationFlag;
}

void Chaos::Physics::Actor::SetZRotationConstraint(bool p_rotationFlag)
{
	if (m_type != ActorType::Dynamic || !m_rigidActor || m_rotationConstraints.y == p_rotationFlag)
		return;

	if (p_rotationFlag)
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	
	else
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, false);

	m_rotationConstraints.z = p_rotationFlag;
}

void Chaos::Physics::Actor::SetXTranslationConstraint(bool p_translationFlag)
{
	if (m_type != ActorType::Dynamic || !m_rigidActor || m_rotationConstraints.x == p_translationFlag)
		return;

	if (p_translationFlag)
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
	
	else
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, false);

	m_translationConstraint.x = p_translationFlag;
}

void Chaos::Physics::Actor::SetYTranslationConstraint(bool p_translationFlag)
{
	if (m_type != ActorType::Dynamic || !m_rigidActor || m_rotationConstraints.y == p_translationFlag)
		return;

	if (p_translationFlag)
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, true);
	
	else
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, false);

	m_translationConstraint.y = p_translationFlag;
}

void Chaos::Physics::Actor::SetZTranslationConstraint(bool p_translationFlag)
{
	if (m_type != ActorType::Dynamic || !m_rigidActor || m_rotationConstraints.z == p_translationFlag)
		return;

	if (p_translationFlag)
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);
	
	else
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, false);

	m_translationConstraint.z = p_translationFlag;
}

void Chaos::Physics::Actor::SetKinematic(bool p_isKinematic)
{
	if (!m_rigidActor || m_isKinematic == p_isKinematic)
		return;

	if (p_isKinematic)
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

	else
		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);

	m_isKinematic = p_isKinematic;
}

void Chaos::Physics::Actor::SetID(uint64_t p_id)
{
	m_ID = p_id;
	m_rigidActor->setName(std::to_string(p_id).c_str());
}

void Chaos::Physics::Actor::SetUpCollisionFilteringData(uint32_t p_actorMask, uint32_t p_toBlock)
{
	physx::PxFilterData filterData;
	filterData.word0 = p_actorMask;
	filterData.word1 = p_toBlock;	
	const physx::PxU32 numShapes = this->PhysicsActor().getNbShapes();
	physx::PxShape** shapes = new(physx::PxShape*[numShapes]);
	this->PhysicsActor().getShapes(shapes, numShapes);
	for (physx::PxU32 i = 0; i < numShapes; i++)
	{
		physx::PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	delete(shapes);
}

void Chaos::Physics::Actor::AddForce(physx::PxVec3 p_force, ForceType p_forceType)
{
	if (m_type == ActorType::Dynamic)
	{
		physx::PxForceMode::Enum mode;
		if (p_forceType == ForceType::ACCELERATION)
			mode = physx::PxForceMode::eACCELERATION;
		else if (p_forceType == ForceType::FORCE)
			mode = physx::PxForceMode::eFORCE;
		else if (p_forceType == ForceType::IMPULSE)
			mode = physx::PxForceMode::eIMPULSE;

		static_cast<physx::PxRigidDynamic*>(m_rigidActor)->addForce(p_force, mode);
	}
}

void Chaos::Physics::Actor::EnableDiscretCollisionDetection()
{
	if (m_discreteCollision)
		return;

	m_discreteCollision = true;
	static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, false);
}

void Chaos::Physics::Actor::EnableCCDCollisionDetection()
{
	if (!m_discreteCollision)
		return;

	m_discreteCollision = false;
	static_cast<physx::PxRigidDynamic*>(m_rigidActor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
}

void Chaos::Physics::Actor::EnableGravity() 
{
	if (m_useGravity)
		return;

	m_useGravity = true;
	m_rigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
}

void Chaos::Physics::Actor::DisableGravity() 
{
	if (!m_useGravity)
		return;

	m_useGravity = false;
	m_rigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
}

Chaos::Physics::RotationConstraint& Chaos::Physics::Actor::GetRotationConstraint()
{
	return m_rotationConstraints;
}

Chaos::Physics::TranslationConstraint& Chaos::Physics::Actor::GetTranslationConstraint()
{
	return m_translationConstraint;
}

bool& Chaos::Physics::Actor::GetisKinematic()
{
	return m_isKinematic;
}

void Chaos::Physics::Actor::DeleteActor() const
{
	if (m_rigidActor)
	{
		physx::PxU32 shapes = m_rigidActor->getNbShapes();
		physx::PxShape** GetShapes = new physx::PxShape*[shapes];
		m_rigidActor->getShapes(GetShapes, shapes);
		for (physx::PxU32 i = 0; i < shapes; ++i)
		{
			m_rigidActor->detachShape(*GetShapes[i]);
		}
		physx::PxScene* currentScene = m_rigidActor->getScene();
		currentScene->removeActor(*m_rigidActor);
		m_rigidActor->release();
		delete GetShapes;
	}
}
