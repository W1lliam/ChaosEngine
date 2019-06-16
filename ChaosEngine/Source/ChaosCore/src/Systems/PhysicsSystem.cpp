#include "ChaosCore/Systems/PhysicsSystem.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosPhysics/Maths/Conversion.h"
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosCore/Components/TransformComponent.h"
#include "ChaosCore/Physics/Collider.h"

namespace Chaos
{
    CHAOS_DEFINE_SYSTEM_TYPEID(PhysicsSystem);

    void PhysicsSystem::FixedUpdate(double p_fixedDeltaTime)
    {
        ScenePtr scene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
        scene->GetPhysicsScene().SimulatePhysics(static_cast<float>(p_fixedDeltaTime));

        for (auto rbIt = scene->begin<RigidBodyComponent>(); rbIt != scene->end<RigidBodyComponent>(); rbIt++)
        {
            RigidBodyComponent* rigidComp = static_cast<RigidBodyComponent*>(rbIt->second);
            if (rigidComp->GetRigidBodyType() == E_RigidBodyType::DYNAMIC)
            {
                Transform transformation = Physics::Converter::ConvertPxTransformToTransform(rigidComp->GetActor().GetPhysicsActor().getGlobalPose());
                EntityPtr entity = EntityManager::Get().GetEntity(rigidComp->GetOwnerID());
                TransformComponent* transform = entity->GetComponent<TransformComponent>();
                Maths::Vector3 scale = transform->AsTransform().GetScale();
                transform->SetTransform(transformation);
                transform->AsTransform().SetScale(scale);

            }
        }
    }
    System& PhysicsSystem::Load(ReadStream& p_stream)
    {
        PhysicsSystem* system = SystemManager::Get().AddSystem<PhysicsSystem>();
        static_cast<System*>(system)->Load(p_stream);
        return *system;
    }
	void PhysicsSystem::Update(double p_deltaTime)
	{
	}

	void PhysicsSystem::UpdateMask()
	{

	}
	
	void PhysicsSystem::AddCollider(Collider& p_collider)
	{
		m_colliderList.emplace_back(&p_collider);
	}

	void PhysicsSystem::AddNewMask(std::string p_collisionMask)
	{
		for (size_t i = 0; i < m_collisionMaskList.size(); ++i)
		{
			if (m_collisionMaskList[i] == p_collisionMask)
				return;
		}
		m_collisionMaskList.emplace_back(p_collisionMask);
	}

	void PhysicsSystem::DeleteMask(std::string p_collisionMask)
	{
		for (size_t i = 0; i < m_collisionMaskList.size(); ++i)
		{
			if (m_collisionMaskList[i] == p_collisionMask)
			{
				m_collisionMaskList.erase(m_collisionMaskList.begin() + i);
			}
		}
	}

	std::vector<Collider*>& PhysicsSystem::GetColliderList()
	{
		return m_colliderList;
	}

	std::vector<std::string>& PhysicsSystem::GetCollisionMask()
	{
		return m_collisionMaskList;
	}
}