#include "ChaosCore/Physics/Collider.h"
#include "ChaosPhysics/Base/PhysicsElementCreator.h"
#include "ChaosCore/ECS/Managers/SystemManager.h"
#include "ChaosCore/Systems/SceneSystem.h"
#include "ChaosCore/Components/RigidBodyComponent.h"
#include "ChaosCore/Components/BoxColliderComponent.h"
#include "ChaosCore/Components/capsuleColliderComponent.h"
#include "ChaosCore/Components/MeshComponent.h"
#include "ChaosCore/Components/SphereColliderComponent.h"
#include "ChaosCore/Systems/PhysicsSystem.h"

namespace Chaos
{
	Collider::Collider(ColliderType p_colliderType, Maths::Vector3 p_fiction, Maths::Vector3 p_offSet) :
		m_type(p_colliderType),
		m_friction(p_fiction.x, p_fiction.y, p_fiction.z),
		m_rigidBodyOffSet(p_offSet)
	{
		SystemManager::Get().GetSystem<PhysicsSystem>()->AddCollider(*this);
	}

	Collider::~Collider()
	{
	}

	void Collider::DestroyPhysicsShape() const
	{
		if (m_shape->GetShape()->getActor())
		{
			m_shape->GetShape()->getActor()->detachShape(*m_shape->GetShape());
		}
		m_shape->GetShape()->release();
		delete m_shape;
		for (size_t i = 0; i < SystemManager::Get().GetSystem<PhysicsSystem>()->GetColliderList().size(); ++i)
		{
			if (SystemManager::Get().GetSystem<PhysicsSystem>()->GetColliderList()[i] == this)
			{
				SystemManager::Get().GetSystem<PhysicsSystem>()->GetColliderList().erase(SystemManager::Get().GetSystem<PhysicsSystem>()->GetColliderList().begin()  +i);
			}
		}
	}

	void Collider::ChangeOffset(float p_x, float p_y, float p_z)
	{
		if (m_shape)
		{
			physx::PxTransform transform(physx::PxVec3(p_x, p_y, p_z));
			m_shape->SetTransform(transform);
			m_rigidBodyOffSet = Physics::Converter::ConvertPxVec3ToVector3(transform.p);
		}
	}

	void Collider::ChangeFriction(float p_staticFriction, float p_dynamicFriction, float p_restitution)
	{

		if (m_shape)
		{
			physx::PxMaterial* toModify = nullptr;
			m_shape->GetShape()->getMaterials(&toModify, 1);
			m_friction.SetFrictions(p_staticFriction, p_dynamicFriction, p_restitution);
			toModify->setDynamicFriction(static_cast<physx::PxReal>(p_dynamicFriction));
			toModify->setRestitution(static_cast<physx::PxReal>(p_restitution));
			toModify->setStaticFriction(static_cast<physx::PxReal>(p_staticFriction));
		}
	}

	Maths::Vector3& Collider::GetPosition()
	{
		return m_rigidBodyOffSet;
	}
	Maths::Vector3& Collider::GetFrictionVector()
	{
		return m_friction.GetFrictions();
	}

	void Collider::ReAttachStaticActor() const
	{
		physx::PxRigidActor* actor = m_shape->GetShape()->getActor();
		physx::PxScene* scene = nullptr;
		Physics::ElementCreator::Get().GetPxPhysics()->getScenes(&scene, 1);
		physx::PxTransform transform(physx::PxVec3(0, 0, 0), physx::PxQuat(0, physx::PxVec3(0, 0, 0)));
		actor->detachShape(*this->GetPhysicsShape()->GetShape());
		physx::PxRigidStatic* staticActor = Physics::ElementCreator::Get().GetPxPhysics()->createRigidStatic(transform);
		staticActor->attachShape(*m_shape->GetShape());
		scene->removeActor(*actor);
		scene->addActor(*staticActor);
		actor->release();
	}

	void Collider::OnCollisionEnter(CollisionInfo & p_collisionInfo)
	{

	}

	void Collider::OnCollisionStay(CollisionInfo & p_collisionInfo)
	{
	}

	void Collider::OnCollisionExit(CollisionInfo & p_collisionInfo)
	{
	}

	void Collider::OnTriggerEnter(TriggerInfo & p_collisionInfo)
	{
	}

	void Collider::OnTriggerExit(TriggerInfo & p_collisionInfo)
	{
	}

	void Collider::onContact(const physx::PxContactPairHeader & pairHeader, const physx::PxContactPair * pairs, physx::PxU32 nbPairs)
	{
		for (physx::PxU32 i = 0; i < nbPairs; i++)
		{
			const physx::PxContactPair& cp = pairs[i];
			if (cp.shapes[0] == this->GetPhysicsShape()->GetShape() || cp.shapes[1] == this->GetPhysicsShape()->GetShape())
			{
				physx::PxShape* otherShape = (this->GetPhysicsShape()->GetShape() == cp.shapes[0]) ? cp.shapes[1] : cp.shapes[0];
				physx::PxActor* otherActor = otherShape->getActor();
				ScenePtr currentScene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
				CollisionInfo info;
				currentScene->GetEntities().find_if(([otherActor, &info](Scene::SceneNode& p_node)
				{
					bool flag = false;
					RigidBodyComponent* compo = p_node.entity->GetComponent<RigidBodyComponent>();
					uint64_t id = std::stoll(otherActor->getName());
					if (compo->Actor().GetID() == id)
					{
						info.CollidedRigidBody = compo;
						info.CollidedCollider = compo->GetAttachedCollider();
						flag = true;
					}
					return flag;
					
				}));				
				info.numberOfContact = cp.contactCount;
				physx::PxContactPairPoint* buffer = new physx::PxContactPairPoint[cp.contactCount];
				cp.extractContacts(buffer, cp.contactCount);
				for (physx::PxU32 i = 0; i < cp.contactCount; i++)
				{
					info.contactPoints.emplace_back(Physics::Converter::ConvertPxVec3ToVector3(buffer->position), Physics::Converter::ConvertPxVec3ToVector3(buffer->normal), static_cast<float>(buffer->separation));
				}
				if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					OnCollisionEnter(info);
				}
				else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					OnCollisionStay(info);
				}
				else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
				{
					OnCollisionExit(info);
				}
			}
		}
	}
	void Collider::onTrigger(physx::PxTriggerPair * pairs, physx::PxU32 count)
	{
		for (physx::PxU32 i = 0; i < count; i++)
		{
			if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

			
			const physx::PxTriggerPair& triggerPairInfo = pairs[i];
			if (triggerPairInfo.triggerShape == this->GetPhysicsShape()->GetShape() || triggerPairInfo.otherShape == this->GetPhysicsShape()->GetShape())
			{
				physx::PxShape* otherShape = (this->GetPhysicsShape()->GetShape() == triggerPairInfo.triggerShape) ? triggerPairInfo.triggerShape : triggerPairInfo.otherShape;
				physx::PxActor* otherActor = otherShape->getActor();
				ScenePtr currentScene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
				TriggerInfo info;
				currentScene->GetEntities().find_if(([otherActor, &info](Scene::SceneNode& p_node)
				{
					bool flag = false;
					RigidBodyComponent* compo = p_node.entity->GetComponent<RigidBodyComponent>();
					uint64_t id = std::stoll(otherActor->getName());
					if (compo->Actor().GetID() == id)
					{
						info.CollidedRigidBody = compo;
						info.CollidedCollider = compo->GetAttachedCollider();
						flag = true;
					}
					return flag;
				}));

				if (triggerPairInfo.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					OnTriggerEnter(info);
				}
				else if (triggerPairInfo.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					OnTriggerExit(info);
				}
			}
		}
	}
}