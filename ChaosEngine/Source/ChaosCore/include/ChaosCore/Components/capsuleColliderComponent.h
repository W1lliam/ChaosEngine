#pragma once
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/Physics/Collider.h"

namespace Chaos
{
	class CapsuleColliderComponent :public IComponent, public Collider
	{
        CHAOS_COMPONENT
	public:
		CapsuleColliderComponent(COMPONENT_INIT_PARAMS, Maths::Vector3 p_friction, Maths::Vector3 p_offSet, float p_radius, float p_halfHeight);
		void CreateShape(uint64_t p_id);

		//getter
		float GetRadius() const;
		float GetHalfHeight() const;
		//setter
		void SetRadius(float p_radius);
		void SetHalfHeight(float p_halfHeight);

		virtual bool        Save(WriteStream& p_stream) override;
		static  IComponent& Load(ReadStream& p_stream, const EntityID& p_entityID);

	private:
		float m_radius;
		float m_halfHeight;
	};
}