#include "ChaosCore/Components/SphereColliderComponent.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosPhysics/Base/PhysicsElementCreator.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Systems/PhysicsSystem.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Scene/Scene.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Scripting/ScriptsHandler.h"
namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(SphereColliderComponent);

    SphereColliderComponent::SphereColliderComponent(COMPONENT_INIT_PARAMS, Maths::Vector3 p_friction, Maths::Vector3 p_offSet, float p_radius) :
        COMPONENT_INIT(Chaos::SphereColliderComponent),
        Collider(ColliderType::SPHERE, p_friction, p_offSet),
        m_radius(p_radius)
    {
        CreateShape(p_componentID);
    }

	float& SphereColliderComponent::GetRadius()
    {
        return m_radius;
    }

    void SphereColliderComponent::SetRadius(float p_radius)
    {
		if (m_shape)
		{
			m_radius = p_radius;
			physx::PxSphereGeometry geometry;
			m_shape->GetShape()->getSphereGeometry(geometry);
			geometry.radius = static_cast<physx::PxReal>(p_radius);
			m_shape->GetShape()->setGeometry(geometry);
		}
    }

    void SphereColliderComponent::CreateShape(uint64_t p_id)
    {
        m_shape = Physics::Shape::CreateSphereShape(m_radius, Physics::Friction(m_friction.GetStaticFriction(), m_friction.GetDynamicFriction(), m_friction.GetRestitution()));
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

	bool SphereColliderComponent::Save(WriteStream & p_stream)
    {
        SerializeUnsignedInt(p_stream, (uint8_t&)m_type, ColliderType::EMPTY, ColliderType::SPHERE);
        auto frictions = m_friction.GetFrictions();
        SerializeVector3(p_stream, frictions);
        SerializeVector3(p_stream, m_rigidBodyOffSet);
        SerializeFloat(p_stream, m_radius);
        return true;
    }

    IComponent& SphereColliderComponent::Load(ReadStream & p_stream, const EntityID & p_entityID)
    {
        ColliderType type;
        Maths::Vector3 friction;
        Maths::Vector3 rigidBodyOffSet;
        float radius;

        SerializeUnsignedInt(p_stream, (uint8_t&)type, ColliderType::EMPTY, ColliderType::SPHERE);
        SerializeVector3(p_stream, friction);
        SerializeVector3(p_stream, rigidBodyOffSet);
        SerializeFloat(p_stream, radius);
        IComponent& comp = ComponentManager::Get().AddComponent<SphereColliderComponent>(p_entityID, friction, rigidBodyOffSet, radius);
        return comp;
    }
    void SphereColliderComponent::Expose()
    {
        ssq::Class sqSphereCollider = ScriptsHandler::Get().GetVirtualMachine().addClass("SphereColliderComponent",
            [](EntityID p_ownerID, ComponentID p_compID, Maths::Vector3 p_friction, Maths::Vector3 p_offSet, float p_radius) -> SphereColliderComponent*
        {
            return new SphereColliderComponent(p_ownerID, p_compID, p_friction, p_offSet, p_radius);
        });
        sqSphereCollider.addFunc("GetRadius", &SphereColliderComponent::GetRadius);
        sqSphereCollider.addFunc("SetRadius", &SphereColliderComponent::SetRadius);
    }
}