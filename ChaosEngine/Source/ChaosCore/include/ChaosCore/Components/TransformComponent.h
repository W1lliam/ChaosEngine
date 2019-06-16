#pragma once
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/Transform.h"

#ifdef CHAOS_EDITOR
namespace ChaosEditor { class TransformComponentGUI; }
#endif

namespace Chaos
{
    class TransformComponent : public IComponent
    {
        CHAOS_COMPONENT
#ifdef CHAOS_EDITOR
        friend class ::ChaosEditor::TransformComponentGUI;
#endif
    public:
        TransformComponent(COMPONENT_INIT_PARAMS) : COMPONENT_INIT(TransformComponent)
        {}
        ~TransformComponent() = default;


        inline void SetTransform(const Transform& transform) { m_transform = transform; }
        inline Transform& AsTransform() { return m_transform; }
        inline Transform& operator->() { return m_transform; }

        inline Chaos::Maths::Vector3&      GetPositionRef() { return m_transform.m_position; }
        inline Chaos::Maths::Quaternion&   GetRotationRef() { return m_transform.m_rotation; }
        inline Chaos::Maths::Vector3&      GetScaleRef()    { return m_transform.m_scale; }


        virtual bool        Save(WriteStream& p_stream) override;
        static  IComponent& Load(ReadStream& p_stream, const EntityID& p_entityID);
        static  void        Expose();

    private:
        Transform m_transform;
    };
}