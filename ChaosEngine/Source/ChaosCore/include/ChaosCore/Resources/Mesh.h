#pragma once
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Resources/SubMesh.h"

namespace Chaos::Resources
{
    class Mesh : public AResource
    {
        friend class SubMesh;
    public:
        Mesh(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group);
        virtual ~Mesh();

        inline       std::vector<SubMeshPtr>&   GetSubMeshes()       { return m_subMeshes; };
        inline const std::vector<SubMeshPtr>&   GetSubMeshes() const { return m_subMeshes; };
                     SubMeshPtr                 GetSubMesh(const std::string& p_subMeshName) const;

		void								  MakeAsQuad();

    protected:
        virtual void PrepareImpl()      override;
        virtual void LoadImpl()         override;
        virtual void PostLoadImpl()     override;

        virtual void UnprepareImpl()    override;
        virtual void UnloadImpl()       override;

		void MakeLine(Maths::Vector3 p_start, Maths::Vector3 p_end);

    protected:
        std::vector<SubMeshPtr>     m_subMeshes;
        std::ifstream               m_dataStream{};
    };

    using MeshPtr = std::shared_ptr<Mesh>;
}