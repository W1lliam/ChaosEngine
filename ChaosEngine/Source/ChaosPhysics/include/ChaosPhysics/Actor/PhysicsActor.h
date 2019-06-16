#pragma once
#include "ChaosPhysics/Shape/Shape.h"

namespace Chaos
{
	namespace Physics
	{
		enum class ActorType
		{
			Empty,
			Static,
			Dynamic
		};
		enum class ForceType
		{
			ACCELERATION = physx::PxForceMode::eACCELERATION,
			FORCE = physx::PxForceMode::eFORCE,
			IMPULSE = physx::PxForceMode::eIMPULSE
		};

		struct RotationConstraint
		{
			bool x = false;
			bool y = false;
			bool z = false;
		};
		struct TranslationConstraint
		{
			bool x = false;
			bool y = false;
			bool z = false;
		};

		class Actor
		{
		public:
            Actor(ActorType p_type, physx::PxVec3 p_position, physx::PxQuat p_rotation, physx::PxVec3 p_velocity = {}, float p_mass = 0.0f);
            Actor(ActorType p_type, physx::PxTransform p_transform, physx::PxVec3 p_velocity = {}, float p_mass = 0.0f);
			~Actor();

			void AddShape(Shape* p_shape) const;
			void RemoveShape(Shape* p_shape) const;
			void DeleteActor() const;

            inline  void SetActor(physx::PxRigidActor* p_actor)             { m_rigidActor = p_actor; };
            inline  void SetType(ActorType p_type)                          { m_type = p_type; };
                    void SetAngularVelocity(const Maths::Vector3& p_angularVelocity);
                    void SetVelocity(const Maths::Vector3& p_velocity);
                    void SetMass(float p_mass);
                    void SetTransform(physx::PxTransform p_transform);
            
                    physx::PxVec3               GetVelocity()       const;
                    float                       GetMass()           const;
            inline  physx::PxVec3               GetPosition()       const   { return m_rigidActor->getGlobalPose().p; };
            inline  physx::PxQuat               GetRotation()       const   { return m_rigidActor->getGlobalPose().q; };
            inline  ActorType                   GetType()           const   { return m_type; };
            inline  const physx::PxRigidActor&  GetPhysicsActor()   const   { return *m_rigidActor; };
            inline  physx::PxRigidActor&        PhysicsActor()              { return *m_rigidActor; };
			inline  uint64_t					GetID()			const		{ return m_ID; };
					RotationConstraint&         GetRotationConstraint();
					TranslationConstraint&      GetTranslationConstraint();
					bool&					    GetisKinematic();

			void SetXRotationConstraint(bool p_rotationFlag);
			void SetYRotationConstraint(bool p_rotationFlag);
			void SetZRotationConstraint(bool p_rotationFlag);
			void SetXTranslationConstraint(bool p_translationFlag);
			void SetYTranslationConstraint(bool p_translationFlag);
			void SetZTranslationConstraint(bool p_translationFlag);
			void SetKinematic(bool p_isKinematic);
			void SetID(uint64_t p_id);
			void SetUpCollisionFilteringData(uint32_t p_actorMask, uint32_t p_toBlock);

			void AddForce(physx::PxVec3 p_force, ForceType p_forceType);
			void EnableDiscretCollisionDetection();
			void EnableCCDCollisionDetection();
			void EnableGravity();
			void DisableGravity();
		protected:
			bool					m_discreteCollision = true;
			bool					m_useGravity    = false;
			bool                    m_isKinematic   = false;
			RotationConstraint      m_rotationConstraints;
			TranslationConstraint   m_translationConstraint;
			physx::PxRigidActor*    m_rigidActor    = nullptr;
			ActorType               m_type          = ActorType::Empty;
			uint64_t				m_ID			= 0;
		};
	}
}
