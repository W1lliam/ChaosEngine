#pragma once
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/Physics/Collider.h"

#ifdef CHAOS_EDITOR
namespace ChaosEditor { class BoxColliderComponentGUI; }
#endif


namespace Chaos
{
	class BoxColliderComponent :public IComponent, public Collider
	{
        CHAOS_COMPONENT

#ifdef CHAOS_EDITOR
            friend class ::ChaosEditor::BoxColliderComponentGUI;
#endif
	public:
		BoxColliderComponent(COMPONENT_INIT_PARAMS, Maths::Vector3 p_friction, Maths::Vector3 p_offSet, Maths::Vector3 p_halfExtends);
		Maths::Vector3 GetHalfExtends() const;
		void CreateShape(uint64_t p_id);
		void SetHalfExtends(float p_x, float p_y, float p_z);
		void SetHalfExtends(Maths::Vector3 p_halfExtends);

        virtual bool        Save(WriteStream& p_stream) override;
        static  IComponent& Load(ReadStream& p_stream, const EntityID& p_entityID);

	private:
		Maths::Vector3 m_halfExtends;
	};
}