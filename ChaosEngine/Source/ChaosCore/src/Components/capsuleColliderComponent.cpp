#include "ChaosCore/Components/capsuleColliderComponent.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosPhysics/Base/PhysicsElementCreator.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Systems/PhysicsSystem.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Scene/Scene.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(CapsuleColliderComponent);

        CapsuleColliderComponent::CapsuleColliderComponent(COMPONENT_INIT_PARAMS, Maths::Vector3 p_friction,
            Maths::Vector3 p_offSet, float p_radius, float p_halfHeight) :
        COMPONENT_INIT(Chaos::CapsuleColliderComponent), Collider(ColliderType::CAPSULE, p_friction, p_offSet),
        m_radius(p_radius), m_halfHeight(p_halfHeight)
    {
			CreateShape(p_componentID);
    }

    void CapsuleColliderComponent::CreateShape(uint64_t p_id)
    {
        m_shape = Physics::Shape::CreateCapsuleShape(m_radius, m_halfHeight, Physics::Friction(m_friction.GetStaticFriction(), m_friction.GetDynamicFriction(), m_friction.GetRestitution()));
		RigidBodyComponent* rigidBody = nullptr;
		rigidBody = ComponentManager::Get().GetComponent<RigidBodyComponent>(m_owner_ID);
		if (!rigidBody)
		{
			Maths::Vector3 pos = ComponentManager::Get().GetComponent<TransformComponent>(m_owner_ID)->AsTransform().GetPosition();
			ScenePtr scene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
			physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(0, physx::PxVec3(0, 0, 0)));
			physx::PxRigidStatic* staticActor = Physics::ElementCreator::Get().GetPxPhysics()->createRigidStatic(transform);
			staticActor->attachShape(*m_shape->GetShape());
			scene->GetPhysicsScene().GetScene()->addActor(*staticActor);
		}
		else
		{
			physx::PxRigidActor* actor = &rigidBody->Actor().PhysicsActor();
			rigidBody->AttachCollider(this);
			actor->attachShape(*m_shape->GetShape());
		}
		m_shape->SetID(p_id);
		SystemManager::Get().GetSystem<PhysicsSystem>()->AddCollider(*this);
    }

    float CapsuleColliderComponent::GetRadius() const
    {
        return m_radius;
    }

    float CapsuleColliderComponent::GetHalfHeight() const
    {
        return m_halfHeight;
    }

    void CapsuleColliderComponent::SetRadius(float p_radius)
    {
        m_radius = p_radius;
    }

    void CapsuleColliderComponent::SetHalfHeight(float p_halfHeight)
    {
        m_halfHeight = p_halfHeight;
    }

    bool CapsuleColliderComponent::Save(WriteStream& p_stream)
    {
        SerializeUnsignedInt(p_stream, (uint8_t&)m_type, ColliderType::EMPTY, ColliderType::CAPSULE);
        SerializeVector3(p_stream, m_friction.GetFrictions());
        SerializeVector3(p_stream, m_rigidBodyOffSet);
        SerializeFloat(p_stream, m_radius);
        SerializeFloat(p_stream, m_halfHeight);
        return true;
    }

    IComponent& Chaos::CapsuleColliderComponent::Load(ReadStream& p_stream, const EntityID& p_entityID)
    {
        ColliderType type;
        Maths::Vector3 friction;
        Maths::Vector3 rigidBodyOffSet;
        float radius;
        float halfHeight;

        SerializeUnsignedInt(p_stream, (uint8_t&)type, ColliderType::EMPTY, ColliderType::CAPSULE);
        SerializeVector3(p_stream, friction);
        SerializeVector3(p_stream, rigidBodyOffSet);
        SerializeFloat(p_stream, radius);
        SerializeFloat(p_stream, halfHeight);
        IComponent& comp = ComponentManager::Get().AddComponent<CapsuleColliderComponent>(p_entityID, friction, rigidBodyOffSet, radius, halfHeight);
        return comp;
    }
}