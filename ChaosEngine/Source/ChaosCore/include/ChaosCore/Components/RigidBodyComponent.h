#pragma once
#include <ChaosMaths.h>
#include <ChaosPhysics/Actor/PhysicsActor.h>
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/ECS/Component/Component.h"
namespace Chaos
{
	class SphereColliderComponent;
	class BoxColliderComponent;
	class Collider;
	enum class E_RigidBodyType : uint8_t
	{
		EMPTY       = (uint8_t)Physics::ActorType::Empty,
		STATIC      = (uint8_t)Physics::ActorType::Static,
		DYNAMIC     = (uint8_t)Physics::ActorType::Dynamic
	};

	class RigidBodyComponent : public IComponent
	{
        CHAOS_COMPONENT
	public:
        explicit RigidBodyComponent(COMPONENT_INIT_PARAMS, E_RigidBodyType p_type, float p_mass = 0.0f, Maths::Vector3 p_velocity = {});

		~RigidBodyComponent() override;

        inline float&                   GetMass()           { return m_mass; }
        inline float&                   GetAngularDrag()    { return m_angularDrag; }
		inline float&					GetLinearDrag()		{ return m_linearDrag;  }
        inline Maths::Vector3           GetVelocity()       const { return Physics::Converter::ConvertPxVec3ToVector3(m_rigidActor->GetVelocity()); }
        inline E_RigidBodyType          GetRigidBodyType()  const { return m_type; }
        inline bool                     IsKinematic()       const { return m_isKinematic; }
        inline const Physics::Actor&    GetActor()          const { return *m_rigidActor; }
        inline Physics::Actor&          Actor()             const { return *m_rigidActor; }
			   Collider*			    GetAttachedCollider();
		inline bool						IsGravityEnabled() const { return m_isGravityEnabled; };
			   bool&					GetXRotationConstraint() const;
			   bool&					GetYRotationConstraint() const;
			   bool&					GetZRotationConstraint() const;
			   bool&					GetXTranslationConstraint() const;
			   bool&					GetYTranslationConstraint() const;
			   bool&					GetZTranslationConstraint() const;

			   void SetMass(float p_mass);
			   void SetAngularDrag(float p_angularDrag);
			   void SetLinearDrag(float p_linearDrag);
        inline void SetGlobalTransform(const Transform& p_transform)                { m_rigidActor->SetTransform(Physics::Converter::ConvertTransformToPxTransform(p_transform)); };
        inline void SetAngularVelocity(const Maths::Vector3& p_angularVelocity)     { m_rigidActor->SetAngularVelocity(p_angularVelocity); };
        inline void SetAngularVelocity(float p_x, float p_y, float p_z)             { m_rigidActor->SetAngularVelocity({ p_x,p_y,p_z }); };
        inline void SetVelocity(const Maths::Vector3& p_velocity)                   { m_rigidActor->SetVelocity(p_velocity); };
        inline void SetVelocity(float p_x, float p_y, float p_z)                    { m_rigidActor->SetVelocity({ p_x,p_y,p_z }); };
        inline void AddForce(const Maths::Vector3& p_force, Physics::ForceType p_forceType = Physics::ForceType::ACCELERATION)         { m_rigidActor->AddForce(Physics::Converter::ConvertVector3ToPxVec3(p_force), p_forceType); };
        inline void AddForce(float p_x, float p_y, float p_z, Physics::ForceType p_forceType = Physics::ForceType::ACCELERATION)       { m_rigidActor->AddForce(Physics::Converter::ConvertVector3ToPxVec3({ p_x,p_y,p_z }), p_forceType); };
        inline void SetRigidBodyType(E_RigidBodyType p_type)        { m_type = p_type; };
			   void SetKinematic(bool p_kinematic);
			   void SetGravity(bool p_enableGraivity);

			   void SetXRotationConstraint(bool p_enableConstraint);
			   void SetYRotationConstraint(bool p_enableConstraint);
			   void SetZRotationConstraint(bool p_enableConstraint);
			   
			   void SetXTranslationConstraint(bool p_enableConstraint);
			   void SetYTranslationConstraint(bool p_enableConstraint);
			   void SetZTranslationConstraint(bool p_enableConstraint);

			   void AttachCollider(Collider* p_collider);
        virtual bool        Save(WriteStream& p_stream) override;
        static IComponent&  Load(ReadStream& p_stream, const EntityID& p_entityID);

        static void         Expose();

	private:
		Physics::Actor* m_rigidActor    = nullptr;
		Collider*		m_attachedCollider = nullptr;
		float           m_angularDrag   = 0;
		float			m_linearDrag	= 0;
		float			m_mass			= 0;
		E_RigidBodyType m_type          = E_RigidBodyType::EMPTY;
		bool            m_isKinematic   = false;
		bool			m_isGravityEnabled = true;
		bool            m_hasACollider  = false;

	};
}