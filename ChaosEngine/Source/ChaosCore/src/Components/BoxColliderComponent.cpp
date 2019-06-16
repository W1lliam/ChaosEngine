#include "ChaosCore/Components/BoxColliderComponent.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosPhysics/Base/PhysicsElementCreator.h"
#include "ChaosCore/Systems/PhysicsSystem.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Scene/Scene.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
namespace Chaos
{
    CHAOS_DEFINE_COMPONENT_TYPEID(BoxColliderComponent);

    BoxColliderComponent::BoxColliderComponent(COMPONENT_INIT_PARAMS, Maths::Vector3 p_friction, Maths::Vector3 p_offSet, Maths::Vector3 p_halfExtends) : 
        COMPONENT_INIT(Chaos::BoxColliderComponent), 
        Collider(ColliderType::BOX, p_friction, p_offSet),
        m_halfExtends(p_halfExtends)
    {
        CreateShape(p_componentID);
    }

    Maths::Vector3 BoxColliderComponent::GetHalfExtends() const
    {
        return m_halfExtends;
    }

    void BoxColliderComponent::CreateShape(uint64_t p_id)
    {
        m_shape = Physics::Shape::CreateBoxShape(m_halfExtends, Physics::Friction(m_friction.GetStaticFriction(), m_friction.GetDynamicFriction(), m_friction.GetRestitution()));
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
	void BoxColliderComponent::SetHalfExtends(float p_x, float p_y, float p_z)
    {
        m_halfExtends = Maths::Vector3(p_x, p_y, p_z);
    }

    void BoxColliderComponent::SetHalfExtends(Maths::Vector3 p_halfExtends)
    {
        m_halfExtends = p_halfExtends;
    }

    bool BoxColliderComponent::Save(WriteStream & p_stream)
    {
        SerializeUnsignedInt(p_stream, (uint8_t&)m_type, ColliderType::EMPTY, ColliderType::BOX);
        SerializeVector3(p_stream, m_friction.GetFrictions());
        SerializeVector3(p_stream, m_rigidBodyOffSet);
        SerializeVector3(p_stream, m_halfExtends);
        return true;
    }

    IComponent& BoxColliderComponent::Load(ReadStream & p_stream, const EntityID & p_entityID)
    {
        ColliderType type;
        Maths::Vector3 friction;
        Maths::Vector3 rigidBodyOffSet;
        Maths::Vector3 halfExtends;

        SerializeUnsignedInt(p_stream, (uint8_t&)type, ColliderType::EMPTY, ColliderType::BOX);
        SerializeVector3(p_stream, friction);
        SerializeVector3(p_stream, rigidBodyOffSet);
        SerializeVector3(p_stream, halfExtends);
        IComponent& comp = ComponentManager::Get().AddComponent<BoxColliderComponent>(p_entityID, friction, rigidBodyOffSet, halfExtends);
        return comp;
    }
}