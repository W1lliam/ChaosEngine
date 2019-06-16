#pragma once
#include "ChaosCore/Physics/Collider.h"
#include "ChaosCore/ECS/Component/Component.h"

namespace Chaos
{
	class SphereColliderComponent : public IComponent, public Collider
	{
        CHAOS_COMPONENT
	public:
		SphereColliderComponent(COMPONENT_INIT_PARAMS, Maths::Vector3 p_friction, Maths::Vector3 p_offSet, float p_radius);
		float& GetRadius();
		void SetRadius(float p_radius);
		void CreateShape(uint64_t p_id);
        virtual bool        Save(WriteStream& p_stream) override;
        static  IComponent& Load(ReadStream& p_stream, const EntityID& p_entityID);
        static void         Expose();

	private:
		float m_radius = 0.f;
	};
}
