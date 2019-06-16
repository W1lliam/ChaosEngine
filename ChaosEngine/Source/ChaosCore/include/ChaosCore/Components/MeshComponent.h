#pragma once
#include "ChaosCore/ECS/Component/Component.h"
#include "ChaosCore/Resources/SubMesh.h"
#include "ChaosCore/Resources/Mesh.h"


#ifdef CHAOS_EDITOR
namespace ChaosEditor { class MeshComponentGUI; }
#endif

namespace Chaos
{
    class MeshComponent : public IComponent
    {
        CHAOS_COMPONENT
#ifdef CHAOS_EDITOR
        friend class ::ChaosEditor::MeshComponentGUI;
#endif
    public:
        MeshComponent(COMPONENT_INIT_PARAMS, Resources::SubMeshPtr p_mesh = {});
        void SetMesh(Resources::SubMeshPtr p_mesh);
        const Resources::SubMeshPtr GetMesh() const;

        virtual bool        Save(WriteStream& p_stream) override;
        static  IComponent& Load(ReadStream& p_stream, const EntityID& p_entityID);

    private:
        Resources::SubMeshPtr m_mesh;
    };
}