#pragma once
#include "ChaosPhysics/Friction/Friction.h"
#include "ChaosPhysics/Maths/Conversion.h"
#include "ChaosPhysics/Collision/CollisionMask.h"

namespace Chaos
{
	namespace Physics
	{
		enum class ColliderType
		{
			BOX,
			CAPSULE,
			MESH,
			SPHERE,
			NONE
		};
		class Shape
		{
		public:
			Shape() = default;
			Shape(physx::PxShape* p_shape);
			~Shape() = default;
			physx::PxShape* GetShape();
			ColliderType GetType() const;
			void SetType(ColliderType p_type);

			static Shape* CreateBoxShape(Maths::Vector3 p_halfExtends, Friction p_friction)
			{
				Shape* toReturn = new Shape(ElementCreator::Get().GetPxPhysics()->createShape(physx::PxBoxGeometry(Converter::ConvertVector3ToPxVec3(p_halfExtends)), *p_friction.GetMaterial(), true));
				toReturn->SetType(ColliderType::BOX);
				return toReturn;
			}
			static Shape* CreateCapsuleShape(float p_radius, float p_halfHeight, Friction p_friction)
			{
				Shape* toReturn = new Shape(ElementCreator::Get().GetPxPhysics()->createShape(physx::PxCapsuleGeometry(p_radius, p_halfHeight), *p_friction.GetMaterial(), true));
				toReturn->SetType(ColliderType::CAPSULE);
				return toReturn;
			}
			static Shape* CreateSphereShape(float p_radius, Friction p_friction)
			{
				Shape* toReturn = new Shape(ElementCreator::Get().GetPxPhysics()->createShape(physx::PxSphereGeometry(p_radius), *p_friction.GetMaterial(), true));
				toReturn->SetType(ColliderType::SPHERE);
				return toReturn;
			}
			void SetTransform(physx::PxTransform p_transform);
			void SetTrigger(bool p_isTrigger);
			void SetID(uint64_t p_id);
			bool& GetIsTrigger();
			inline uint64_t GetID() const { return m_ID; };
		protected:
			physx::PxShape* m_shape = nullptr;
			ColliderType m_type = ColliderType::NONE;
			bool m_isTrigger = false;
		
		private:
			uint64_t m_ID = 0;
			physx::PxTransform m_transformReference;
			void EnableTrigger() const;
			void DisableTrigger() const;
		};
	}
}