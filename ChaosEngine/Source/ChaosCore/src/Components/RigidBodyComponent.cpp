#include "ChaosCore/Components/SphereColliderComponent.h"
#include "ChaosCore/Components/BoxColliderComponent.h"
#include "ChaosCore/Components/capsuleColliderComponent.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Events/PhysicsSceneEvents.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"

namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(RigidBodyComponent);

    RigidBodyComponent::RigidBodyComponent(COMPONENT_INIT_PARAMS, E_RigidBodyType p_type, float p_mass, Maths::Vector3 p_velocity) : 
        COMPONENT_INIT(Chaos::RigidBodyComponent), 
        m_type(p_type), m_mass(p_mass)
    {
        CHAOS_CORE_ASSERT(p_type <= E_RigidBodyType::DYNAMIC, "Invalid RigidBody type");
        
        Collider* collider = nullptr;
        collider = ComponentManager::Get().GetComponent<SphereColliderComponent>(m_owner_ID);
        if(!collider)
            collider = ComponentManager::Get().GetComponent<BoxColliderComponent>(m_owner_ID);

		if (!collider)
			collider = ComponentManager::Get().GetComponent<CapsuleColliderComponent>(m_owner_ID);

		
        Transform transform = EntityManager::Get().GetEntity(this->GetOwnerID())->GetComponent<TransformComponent>()->AsTransform();
        m_rigidActor = new Physics::Actor((Physics::ActorType)p_type, Physics::Converter::ConvertTransformToPxTransform(transform), Physics::Converter::ConvertVector3ToPxVec3(p_velocity), p_mass);
		m_rigidActor->SetID(p_componentID);
		if (collider)
		{
			physx::PxRigidActor* currentOwner = collider->GetPhysicsShape()->GetShape()->getActor();
			currentOwner->detachShape(*collider->GetPhysicsShape()->GetShape());
			physx::PxScene* currentScene = currentOwner->getScene();
			currentScene->removeActor(*currentOwner);
			currentOwner->release();
			m_rigidActor->AddShape(collider->GetPhysicsShape());
			this->AttachCollider(collider);
		}
	
    }

	RigidBodyComponent::~RigidBodyComponent()
	{
	}

	Collider* RigidBodyComponent::GetAttachedCollider()
	{
		if(m_attachedCollider)
			return m_attachedCollider;

		else return nullptr;
	}

	bool & RigidBodyComponent::GetXRotationConstraint() const
	{
		return m_rigidActor->GetRotationConstraint().x;
	}

	bool & RigidBodyComponent::GetYRotationConstraint() const
	{
		return m_rigidActor->GetRotationConstraint().y;
	}

	bool & RigidBodyComponent::GetZRotationConstraint() const
	{
		return m_rigidActor->GetRotationConstraint().z;
	}

	bool & RigidBodyComponent::GetXTranslationConstraint() const
	{
		return m_rigidActor->GetTranslationConstraint().x;
	}

	bool & RigidBodyComponent::GetYTranslationConstraint() const
	{
		return m_rigidActor->GetTranslationConstraint().y;
	}

	bool & RigidBodyComponent::GetZTranslationConstraint() const
	{
		return m_rigidActor->GetTranslationConstraint().z;
	}

	void RigidBodyComponent::SetMass(float p_mass)
	{
		m_mass = p_mass;
		m_rigidActor->SetMass(p_mass);
	}

	void RigidBodyComponent::SetAngularDrag(float p_angularDrag)
	{
		m_angularDrag = p_angularDrag;
		static_cast<physx::PxRigidBody*>(&m_rigidActor->PhysicsActor())->setAngularDamping(p_angularDrag);
	}

	void RigidBodyComponent::SetLinearDrag(float p_linearDrag)
	{
		m_linearDrag = p_linearDrag;
		static_cast<physx::PxRigidBody*>(&m_rigidActor->PhysicsActor())->setLinearDamping(p_linearDrag);
	}

	void RigidBodyComponent::SetKinematic(bool p_kinematic)
	{
		m_isKinematic = p_kinematic;
		m_rigidActor->SetKinematic(p_kinematic);
	}

	void RigidBodyComponent::SetGravity(bool p_enableGraivity)
	{
		m_isGravityEnabled = p_enableGraivity;
		if (p_enableGraivity)
			m_rigidActor->EnableGravity();
		else
			m_rigidActor->DisableGravity();
	}

	void RigidBodyComponent::SetXRotationConstraint(bool p_enableConstraint)
	{
		m_rigidActor->SetXRotationConstraint(p_enableConstraint);
	}

	void RigidBodyComponent::SetYRotationConstraint(bool p_enableConstraint)
	{
		m_rigidActor->SetYRotationConstraint(p_enableConstraint);
	}

	void RigidBodyComponent::SetZRotationConstraint(bool p_enableConstraint)
	{
		m_rigidActor->SetZRotationConstraint(p_enableConstraint);
	}

	void RigidBodyComponent::SetXTranslationConstraint(bool p_enableConstraint)
	{
		m_rigidActor->SetXTranslationConstraint(p_enableConstraint);
	}

	void RigidBodyComponent::SetYTranslationConstraint(bool p_enableConstraint)
	{
		m_rigidActor->SetYTranslationConstraint(p_enableConstraint);
	}

	void RigidBodyComponent::SetZTranslationConstraint(bool p_enableConstraint)
	{
		m_rigidActor->SetZTranslationConstraint(p_enableConstraint);
	}

	void RigidBodyComponent::AttachCollider(Collider * p_collider)
	{
		m_attachedCollider = p_collider;
	}

	bool RigidBodyComponent::Save(WriteStream & p_stream)
    {
        SerializeUnsignedInt(p_stream, (uint8_t&)m_type, (uint8_t)E_RigidBodyType::EMPTY, (uint8_t)E_RigidBodyType::DYNAMIC);
        if (m_type == E_RigidBodyType::DYNAMIC)
        {
            float mass = m_rigidActor->GetMass();
            SerializeFloat(p_stream, mass);
            Maths::Vector3 velocity = Physics::Converter::ConvertPxVec3ToVector3(m_rigidActor->GetVelocity());
            SerializeVector3(p_stream, velocity);
        }

		SerializeFloat(p_stream, m_linearDrag);
        SerializeFloat(p_stream, m_angularDrag);
		SerializeFloat(p_stream, m_mass);
        SerializeBool(p_stream, m_isKinematic);
		SerializeBool(p_stream, m_isGravityEnabled);
        SerializeBool(p_stream, m_hasACollider);
        return true;
    }

    IComponent& RigidBodyComponent::Load(ReadStream & p_stream, const EntityID & p_entityID)
    {
        E_RigidBodyType type;
        SerializeUnsignedInt(p_stream, (uint8_t&)type, (uint8_t)E_RigidBodyType::EMPTY, (uint8_t)E_RigidBodyType::DYNAMIC);

        float mass = 0;
        Maths::Vector3 velocity;
        if (type == E_RigidBodyType::DYNAMIC)
        {
            SerializeFloat(p_stream, mass);
            SerializeVector3(p_stream, velocity);
        }

        RigidBodyComponent& component = ComponentManager::Get().AddComponent<RigidBodyComponent>(p_entityID, type, mass, velocity);
		SerializeFloat(p_stream, component.m_linearDrag);
        SerializeFloat(p_stream, component.m_angularDrag);
		SerializeFloat(p_stream, component.m_mass);
        SerializeBool(p_stream, component.m_isKinematic);
        SerializeBool(p_stream, component.m_isGravityEnabled);
        SerializeBool(p_stream, component.m_hasACollider);

        return component;
    }

    void RigidBodyComponent::Expose()
    {
        ssq::Enum sqRigidBodyType = ScriptsHandler::Get().GetVirtualMachine().addEnum("RigidBodyType");
        sqRigidBodyType.addSlot("EMPTY",    (int)E_RigidBodyType::EMPTY);
        sqRigidBodyType.addSlot("STATIC",   (int)E_RigidBodyType::STATIC);
        sqRigidBodyType.addSlot("DYNAMIC",  (int)E_RigidBodyType::DYNAMIC);

        ssq::Enum sqForceType = ScriptsHandler::Get().GetVirtualMachine().addEnum("ForceType");
        sqForceType.addSlot("ACCELERATION", (int)Physics::ForceType::ACCELERATION);
        sqForceType.addSlot("FORCE",        (int)Physics::ForceType::FORCE);
        sqForceType.addSlot("IMPULSE",      (int)Physics::ForceType::IMPULSE);
        
        ssq::Class sqRigidBody = ScriptsHandler::Get().GetVirtualMachine().addClass("RigidBodyComponent", 
            [](EntityID p_ownerID, ComponentID p_compID, int p_type, float p_mass = 0.0f, Maths::Vector3 p_velocity = {}) -> RigidBodyComponent*
        {
            return new RigidBodyComponent(p_ownerID, p_compID, (E_RigidBodyType)p_type, p_mass, p_velocity);
        });
        sqRigidBody.addFunc("GetMass",          &RigidBodyComponent::GetMass);
        sqRigidBody.addFunc("GetAngularDrag",   &RigidBodyComponent::GetAngularDrag);
		sqRigidBody.addFunc("GetLinearDrag",    &RigidBodyComponent::GetLinearDrag);
        sqRigidBody.addFunc("GetVelocity",      &RigidBodyComponent::GetVelocity);
        sqRigidBody.addFunc("GetRigidBodyType", &RigidBodyComponent::GetRigidBodyType);
        sqRigidBody.addFunc("IsKinematic",      &RigidBodyComponent::IsKinematic);
        sqRigidBody.addFunc("SetKinematic",     &RigidBodyComponent::SetKinematic);
        sqRigidBody.addFunc("SetGravity",       &RigidBodyComponent::SetGravity);

		
        sqRigidBody.addFunc("SetXRotationConstraint",      &RigidBodyComponent::SetXRotationConstraint);
        sqRigidBody.addFunc("SetYRotationConstraint",      &RigidBodyComponent::SetYRotationConstraint);
        sqRigidBody.addFunc("SetZRotationConstraint",      &RigidBodyComponent::SetZRotationConstraint);

        sqRigidBody.addFunc("GetXRotationConstraint",      &RigidBodyComponent::GetXRotationConstraint);
        sqRigidBody.addFunc("GetYRotationConstraint",      &RigidBodyComponent::GetYRotationConstraint);
        sqRigidBody.addFunc("GetZRotationConstraint",      &RigidBodyComponent::GetZRotationConstraint);

        sqRigidBody.addFunc("SetXTranslationConstraint",      &RigidBodyComponent::SetXTranslationConstraint);
        sqRigidBody.addFunc("SetYTranslationConstraint",      &RigidBodyComponent::SetYTranslationConstraint);
        sqRigidBody.addFunc("SetZTranslationConstraint",      &RigidBodyComponent::SetZTranslationConstraint);

        sqRigidBody.addFunc("GetXTranslationConstraint",      &RigidBodyComponent::GetXTranslationConstraint);
        sqRigidBody.addFunc("GetYTranslationConstraint",      &RigidBodyComponent::GetYTranslationConstraint);
        sqRigidBody.addFunc("GetZTranslationConstraint",      &RigidBodyComponent::GetZTranslationConstraint);

        sqRigidBody.addFunc("SetVelocityVec",           [](RigidBodyComponent* p_self, Maths::Vector3 p_velocity) { p_self->SetVelocity(p_velocity); });
        sqRigidBody.addFunc("SetVelocity",              [](RigidBodyComponent* p_self, float x, float y, float z) { p_self->SetVelocity(x, y, z); });

        sqRigidBody.addFunc("SetAngularVelocityVec",    [](RigidBodyComponent* p_self, Maths::Vector3 p_velocity) { p_self->SetAngularVelocity(p_velocity); });
        sqRigidBody.addFunc("SetAngularVelocity",       [](RigidBodyComponent* p_self, float x, float y, float z) { p_self->SetAngularVelocity(x, y, z); });

        sqRigidBody.addFunc("AddForceVec",              [](RigidBodyComponent* p_self, Maths::Vector3 p_velocity, int p_forceType) { p_self->AddForce(p_velocity, (Physics::ForceType)p_forceType); });
        sqRigidBody.addFunc("AddForce",                 [](RigidBodyComponent* p_self, float x, float y, float z, int p_forceType) { p_self->AddForce(x, y, z, (Physics::ForceType)p_forceType); });
    }
}