#pragma once
#include <ChaosMaths.h>
#include <stdint.h>
#include "ChaosCore/ECS/Managers/EntityManager.h"
#include "PhysicsFriction.h"
#include "ChaosPhysics/Shape/Shape.h"
#include "ChaosCore/Physics/CollisionFiltering.h"
namespace Chaos
{
	class RigidBodyComponent;
	class BoxColliderComponent;
	class MeshColliderComponent;
	class SphereColliderComponent;
	class TransformComponent;
	class CapsuleColliderComponent;
	struct CollisionInfo;
	struct TriggerInfo;

	struct ContactPoint
	{
		Maths::Vector3 contactPosition;
		Maths::Vector3 contactNormal;
		float distance;

		ContactPoint(Maths::Vector3 p_contactPosition, Maths::Vector3 p_contactNormal, float p_distance)
		{
			contactPosition = p_contactPosition;
			contactNormal = p_contactNormal;
			distance = p_distance;
		}
	};

	enum ColliderType
	{
		EMPTY,
		BOX,
		CAPSULE,
		MESH,
		SPHERE
	};
	class Collider : public physx::PxSimulationEventCallback
	{
	public:
		Collider(ColliderType p_colliderType, Maths::Vector3 p_fiction, Maths::Vector3 p_offSet);
		~Collider();

                void    DestroyPhysicsShape() const;
		
        inline  void    SetOffSet(Maths::Vector3 p_rigidBodyOffSet)     { m_rigidBodyOffSet = p_rigidBodyOffSet; };
				void	ChangeOffset(float p_x, float p_y, float p_z);
				void	ChangeFriction(float p_staticFriction, float p_dynamicFriction, float p_restitution);
        inline  void    SetColliderType(ColliderType p_type)            { m_type = p_type; };
        inline  void    SetPhysicsFriction(PhysicsFriction p_friction)  { m_friction = p_friction; };
				
					  
					  Maths::Vector3&	  GetPosition();
					  Maths::Vector3&     GetFrictionVector();
        inline  const ColliderType&       GetColliderType() const { return m_type; };
        inline  const Maths::Vector3&     GetOffSet()       const { return m_rigidBodyOffSet; };
        inline  const PhysicsFriction&    GetFriction()     const { return m_friction; };
        inline        Physics::Shape*     GetPhysicsShape() const { return m_shape; };
		inline		  CollisionSetting&   GetCollisionSetting()   { return m_collisionSetting; };
								   void	  ReAttachStaticActor() const;
	protected:
		ColliderType        m_type;
		PhysicsFriction     m_friction;
		Maths::Vector3      m_rigidBodyOffSet;
		Physics::Shape*     m_shape = nullptr;
		CollisionSetting    m_collisionSetting;
		//Collision Functions (Block)
		virtual void OnCollisionEnter(CollisionInfo& p_collisionInfo);
		virtual void OnCollisionStay(CollisionInfo& p_collisionInfo);
		virtual void OnCollisionExit(CollisionInfo& p_collisionInfo);
		
		//Trigger Functions (Overlap/Trigger)
		virtual void OnTriggerEnter(TriggerInfo& p_collisionInfo);
		virtual void OnTriggerExit(TriggerInfo& p_collisionInfo);

	private:
		void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {  };
		void onWake(physx::PxActor** actors, physx::PxU32 count) override {  };
		void onSleep(physx::PxActor** actors, physx::PxU32 count) override {  };
		void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
		void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {  };
	};

	struct CollisionInfo
	{
		Collider* CollidedCollider;
		int numberOfContact;
		RigidBodyComponent* CollidedRigidBody;
		std::vector<ContactPoint> contactPoints;
	};

	struct TriggerInfo
	{
		RigidBodyComponent* CollidedRigidBody;
		Collider* CollidedCollider;
	};
}