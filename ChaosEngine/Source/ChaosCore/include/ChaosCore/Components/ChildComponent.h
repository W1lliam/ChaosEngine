#pragma once
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/Transform.h"

#ifdef CHAOS_EDITOR
namespace ChaosEditor { class ChildComponentGUI; }
#endif

namespace Chaos
{
    class ChildComponent : public IComponent
    {
        CHAOS_COMPONENT
#ifdef CHAOS_EDITOR
        friend class ::ChaosEditor::ChildComponentGUI;
#endif
    public:
        ChildComponent(COMPONENT_INIT_PARAMS, EntityID p_parent);
        ~ChildComponent() = default;

        inline const EntityID&          GetParentID()                                   const   { return m_parent; }

        inline void                     SetCachedTransform(const Maths::Matrix4& p_transform)   { m_cachedTransform = p_transform; }
        inline bool                     SetDirty(const bool p_dirty)                            { m_isDirty = p_dirty; return m_isDirty; }

        inline const Maths::Matrix4&    GetCachedTransform()                            const   { return m_cachedTransform; }
        inline bool                     IsDirty()                                       const   { return m_isDirty; }

        virtual bool            Save(WriteStream& p_stream) override;
        static IComponent&      Load(ReadStream& p_stream, const EntityID& p_entityID);

    private:
        EntityID            m_parent;
        Maths::Matrix4      m_cachedTransform   {};
        bool                m_isDirty           {true};
    };
}