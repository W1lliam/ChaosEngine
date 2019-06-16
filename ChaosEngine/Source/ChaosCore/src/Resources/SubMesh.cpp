#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Resources/Submesh.h"
#include "ChaosCore/Resources/Mesh.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"
#include "ChaosCore/Resources/Managers/MeshManager.h"
#include "ChaosCore/Resources/AssetLoader.h"

namespace Chaos::Resources
{
    SubMesh::SubMesh(Mesh* p_owner) : m_owner(p_owner)
    {
    }

    void SubMesh::Prepare(std::ifstream& p_stream)
    {
        m_state.store(AResource::E_ResourceState::PREPARING);
        CHAOS_CORE_ASSERT(p_stream.is_open(), "Submesh can only load from an opened stream")
        AssetType type;
        p_stream.read(reinterpret_cast<char*>(&type), 1);
        if (type != AssetType::E_ASSETTYPE_SUBMESH)
        {
            CHAOS_CORE_ERROR("Asset type is different from expected type");
            p_stream.close();
            m_state.store(AResource::E_ResourceState::UNPREPARED);
            throw std::exception((std::string("Asset " + m_name) + " is not valid").c_str());
        }
        m_state.store(AResource::E_ResourceState::PREPARED);
    }

    void SubMesh::Load(std::ifstream& p_stream)
    {
        m_state.store(AResource::E_ResourceState::LOADING);
        CHAOS_CORE_ASSERT(p_stream.is_open(), "Asset file has not been opened");
        
        uint32_t blockSize;
        p_stream.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));

        getline(p_stream, m_name, '\0');
        getline(p_stream, m_parent, '\0');


        p_stream.read(reinterpret_cast<char*>(&m_transformation.elements), 16 * sizeof(float));

        uint8_t flag;
        p_stream.read(reinterpret_cast<char*>(&flag), sizeof(flag));

        uint8_t uvCount = 0;
        if (flag & E_VERTEXDATA_UV)
            p_stream.read(reinterpret_cast<char*>(&uvCount), sizeof(uvCount));

        uint8_t colorCount = 0;
        if (flag & E_VERTEXDATA_COLOR)
            p_stream.read(reinterpret_cast<char*>(&colorCount), sizeof(colorCount));

        uint32_t vertexCount = 0;
        p_stream.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));

        if (vertexCount != 0)
        {
            m_loadedVertices.reserve(vertexCount);
            for (uint32_t v = 0; v < vertexCount; ++v)
            {
                Vertex vertex;

                if (flag & E_VERTEXDATA_COORD)
                {
                    p_stream.read(reinterpret_cast<char*>(&vertex.position.x), sizeof(vertex.position.x));
                    p_stream.read(reinterpret_cast<char*>(&vertex.position.y), sizeof(vertex.position.y));
                    p_stream.read(reinterpret_cast<char*>(&vertex.position.z), sizeof(vertex.position.z));
                }

                if (flag & E_VERTEXDATA_NORM)
                {
                    p_stream.read(reinterpret_cast<char*>(&vertex.normal.x), sizeof(vertex.normal.x));
                    p_stream.read(reinterpret_cast<char*>(&vertex.normal.y), sizeof(vertex.normal.y));
                    p_stream.read(reinterpret_cast<char*>(&vertex.normal.z), sizeof(vertex.normal.z));

                    p_stream.read(reinterpret_cast<char*>(&vertex.tangents.x), sizeof(vertex.tangents.x));
                    p_stream.read(reinterpret_cast<char*>(&vertex.tangents.y), sizeof(vertex.tangents.y));
                    p_stream.read(reinterpret_cast<char*>(&vertex.tangents.z), sizeof(vertex.tangents.z));

                    p_stream.read(reinterpret_cast<char*>(&vertex.bitangents.x), sizeof(vertex.bitangents.x));
                    p_stream.read(reinterpret_cast<char*>(&vertex.bitangents.y), sizeof(vertex.bitangents.y));
                    p_stream.read(reinterpret_cast<char*>(&vertex.bitangents.z), sizeof(vertex.bitangents.z));
                }


                for (uint8_t uv = 0; uv < uvCount; ++uv)
                {
                    uint8_t uvComponents = 0;
                    p_stream.read(reinterpret_cast<char*>(&uvComponents), sizeof(uvComponents));

                    float u = 0;
                    float v = 0;
                    float w = 0;

                    p_stream.read(reinterpret_cast<char*>(&u), sizeof(u));
                    p_stream.read(reinterpret_cast<char*>(&v), sizeof(v));

                    if (uvComponents > 2)
                        p_stream.read(reinterpret_cast<char*>(&w), sizeof(w));

                    if(uv == 0)
                        vertex.uv = { u, v, w };
                }

                for (uint8_t colors = 0; colors < colorCount; ++colors)
                {
                    float r;
                    float g;
                    float b;
                    float a;

                    p_stream.read(reinterpret_cast<char*>(&r), sizeof(r));
                    p_stream.read(reinterpret_cast<char*>(&g), sizeof(g));
                    p_stream.read(reinterpret_cast<char*>(&b), sizeof(b));
                    p_stream.read(reinterpret_cast<char*>(&a), sizeof(a));

                    if (colors == 0)
                        vertex.color = Color{ r, g, b, a };
                }                

                m_loadedVertices.push_back(vertex);
            }
        }

        uint32_t faceCount = 0;
        p_stream.read(reinterpret_cast<char*>(&faceCount), sizeof(faceCount));
        m_loadedIndices.reserve(faceCount * 3);

        for (uint32_t face = 0; face < faceCount; ++face)
        {
            for (uint8_t i = 0; i < 3; ++i)
            {
                uint32_t index;
                p_stream.read(reinterpret_cast<char*>(&index), sizeof(index));
                m_loadedIndices.push_back(index);
            }
        }
    }

    void SubMesh::PostLoad()
    {
        using namespace Graphics::Buffers;

        m_vertexBuffer.reset(IVertexBuffer::Create(&m_loadedVertices[0], sizeof(Vertex) * m_loadedVertices.size()));
        m_vertexArray.reset(IVertexArray::Create());
        m_indexBuffer.reset(IIndexBuffer::Create(&m_loadedIndices[0], static_cast<uint32_t>(m_loadedIndices.size())));

        //Position
        m_vertexBuffer->Push(3, CHAOS_FLOAT);
        //Normals
        m_vertexBuffer->Push(3, CHAOS_FLOAT);
        //Tangents
        m_vertexBuffer->Push(3, CHAOS_FLOAT);
        //Bitangents
        m_vertexBuffer->Push(3, CHAOS_FLOAT);
        //UV
        m_vertexBuffer->Push(3, CHAOS_FLOAT);
        //COLOR
        m_vertexBuffer->Push(4, CHAOS_FLOAT);

        m_vertexArray->AddBuffer(*m_vertexBuffer);
        m_state.store(AResource::E_ResourceState::LOADED);
    }

    void SubMesh::Unload()
    {
        m_state.store(AResource::E_ResourceState::UNLOADING);
        m_loadedVertices.clear();
        m_loadedIndices.clear();

        m_vertexArray.reset();
        m_vertexBuffer.reset();
        m_indexBuffer.reset();
        m_state.store(AResource::E_ResourceState::UNPREPARED);
    }
}