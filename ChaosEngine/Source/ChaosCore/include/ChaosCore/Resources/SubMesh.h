#pragma once
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Resources/Buffers/IIndexBuffer.h"
#include "ChaosCore/Resources/Buffers/IVertexArray.h"
#include "ChaosCore/Resources/Buffers/IVertexBuffer.h"
#include "ChaosCore/Resources/Vertex.h"

namespace Chaos::Resources
{

    class SubMesh
    {
    friend class Mesh;
    public:
        SubMesh(Mesh* p_owner);
        SubMesh(SubMesh&& p_other)              = default;
        SubMesh& operator=(SubMesh&& p_other)   = default;
        virtual ~SubMesh()                      = default;
        SubMesh(const SubMesh& p_other)         = delete;

        Graphics::Buffers::IVertexBuffer&   GetVertexBuffer()   const { return *m_vertexBuffer; }
        Graphics::Buffers::IIndexBuffer&    GetIndexBuffer()    const { return *m_indexBuffer; }
        Graphics::Buffers::IVertexArray&    GetVertexArray()    const { return *m_vertexArray; }

        bool                                UseVertexBuffer()   const { return m_vertexBuffer != nullptr; }
        bool                                UseIndexBuffer()    const { return m_indexBuffer != nullptr; }
        bool                                UseVertexArray()    const { return m_vertexArray != nullptr; }

        uint32_t                            GetVertexCount()    const { return static_cast<uint32_t>(m_loadedVertices.size()); }
        const Mesh*                         GetOwner()          const { return m_owner; }
        const std::string&                  GetParent()         const { return m_parent; }
        const std::string&                  GetName()           const { return m_name; }
        Maths::Matrix4                      GetTransformation() const { return m_transformation; }
        AResource::E_ResourceState          GetState()          const { return m_state.load(); }

    protected:
        void Prepare(std::ifstream& p_stream);
        void Load(std::ifstream& p_stream);
        void PostLoad();
        void Unload();

    protected:
        std::vector<Vertex>                         m_loadedVertices{};
        std::vector<uint32_t>                       m_loadedIndices {};
        Mesh*                                       m_owner         {nullptr};
        std::string                                 m_parent        {};
        std::string                                 m_name          {};
        Maths::Matrix4                              m_transformation;
        std::atomic<AResource::E_ResourceState>     m_state{ AResource::E_ResourceState::UNPREPARED };


        std::unique_ptr<Graphics::Buffers::IVertexArray>    m_vertexArray;
        std::unique_ptr<Graphics::Buffers::IVertexBuffer>   m_vertexBuffer;
        std::unique_ptr<Graphics::Buffers::IIndexBuffer>    m_indexBuffer;

    };

    using SubMeshPtr = std::shared_ptr<SubMesh>;
}