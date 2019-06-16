#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Resources/Mesh.h"
#include "ChaosCore/Resources/SubMesh.h"
#include "ChaosCore/Resources/Managers/ResourceGroupManager.h"
#include "ChaosCore/Resources/AssetLoader.h"

namespace Chaos::Resources
{
    Mesh::Mesh(AResourceManager* p_creator, const std::string& p_name, const StringID& p_group) :
        AResource(p_creator, operator""_sid(p_name.c_str(), p_name.size()), p_group)
    {
    }

    Mesh::~Mesh()
    {
        if (m_dataStream.is_open())
        {
            m_dataStream.close();
        }
    }

	void Mesh::MakeAsQuad()
	{
		using namespace Graphics::Buffers;

		m_subMeshes.emplace_back(std::make_shared<SubMesh>(this));

		Vertex BL;
		BL.position =	{ -1.0f, -1.0f, 1.0f };
		BL.uv =			{ 0.0f, 0.0f , 0.0f };
		m_subMeshes[0]->m_loadedVertices.push_back(BL);
		Vertex BR;
		BR.position =	{ 1.0f, -1.0f ,1.0f };
		BR.uv =			{ 1.0f, 0.0f , 0.0f };
		m_subMeshes[0]->m_loadedVertices.push_back(BR);
		Vertex TL;
		TL.position =	{ -1.0f,  1.0 ,1.0f };
		TL.uv =			{ 0.0f, 1.0f , 0.0f };
		m_subMeshes[0]->m_loadedVertices.push_back(TL);
		Vertex TR;
		TR.position =	{ 1.0f,  1.0f ,1.0f };
		TR.uv =			{ 1.0f, 1.0f , 0.0f };
		m_subMeshes[0]->m_loadedVertices.push_back(TR);

		m_subMeshes[0]->m_loadedIndices.push_back(0);
		m_subMeshes[0]->m_loadedIndices.push_back(1);
		m_subMeshes[0]->m_loadedIndices.push_back(2);

		m_subMeshes[0]->m_loadedIndices.push_back(2);
		m_subMeshes[0]->m_loadedIndices.push_back(1);
		m_subMeshes[0]->m_loadedIndices.push_back(3);

		m_subMeshes[0]->m_vertexBuffer.reset(IVertexBuffer::Create(&m_subMeshes[0]->m_loadedVertices[0], sizeof(Vertex) * m_subMeshes[0]->m_loadedVertices.size()));
		m_subMeshes[0]->m_vertexArray.reset(IVertexArray::Create());
		m_subMeshes[0]->m_indexBuffer.reset(IIndexBuffer::Create(&m_subMeshes[0]->m_loadedIndices[0], static_cast<uint32_t>(m_subMeshes[0]->m_loadedIndices.size())));

		//Position
		m_subMeshes[0]->m_vertexBuffer->Push(3, CHAOS_FLOAT);
		//Normals
		m_subMeshes[0]->m_vertexBuffer->Push(3, CHAOS_FLOAT);
		//Tangents
		m_subMeshes[0]->m_vertexBuffer->Push(3, CHAOS_FLOAT);
		//Bitangents
		m_subMeshes[0]->m_vertexBuffer->Push(3, CHAOS_FLOAT);
		//UV
		m_subMeshes[0]->m_vertexBuffer->Push(3, CHAOS_FLOAT);
		//COLOR
		m_subMeshes[0]->m_vertexBuffer->Push(4, CHAOS_FLOAT);

		m_subMeshes[0]->m_vertexArray->AddBuffer(*m_subMeshes[0]->m_vertexBuffer);
		m_subMeshes[0]->m_state.store(E_ResourceState::LOADED);
		m_state.store(E_ResourceState::LOADED);
	}

    SubMeshPtr Mesh::GetSubMesh(const std::string& p_subMeshName) const
    {
        auto subMesh = std::find_if(m_subMeshes.begin(), m_subMeshes.end(), [&p_subMeshName](const SubMeshPtr& p_submesh)
        { return p_submesh->GetName() == p_subMeshName; });

        if (subMesh != m_subMeshes.end())
            return *subMesh;
        return nullptr;
    }

    void Mesh::PrepareImpl()
    {
        m_dataStream = ResourceGroupManager::Get().OpenResource(m_name, m_group);
        
        if (!AssetLoader::Validate(m_dataStream, AssetType::E_ASSETTYPE_MESH))
        {
            throw std::exception((std::string("Asset " + m_name) + " is not valid").c_str());
        }
    }

    void Mesh::LoadImpl()
    {
        CHAOS_CORE_ASSERT(m_dataStream.is_open(), "Asset file has not been opened");

        std::string name;
        getline(m_dataStream, name, '\0');


        uint32_t subMeshCount = 0;
        m_dataStream.read(reinterpret_cast<char*>(&subMeshCount), sizeof(subMeshCount));

        m_subMeshes.reserve(subMeshCount);
        for (uint32_t i = 0; i < subMeshCount; ++i)
        {
            m_subMeshes.emplace_back(std::make_shared<SubMesh>(this));
            m_subMeshes.back()->Prepare(m_dataStream);
            m_subMeshes.back()->Load(m_dataStream);
        }
    }

    void Mesh::PostLoadImpl()
    {
        for (SubMeshPtr subMesh : m_subMeshes)
        {
            subMesh->PostLoad();
        }
    }

    void Mesh::UnprepareImpl()
    {
        if (m_dataStream.is_open())
        {
            m_dataStream.close();
        }
    }

    void Mesh::UnloadImpl()
    {
        for (SubMeshPtr subMesh : m_subMeshes)
        {
            subMesh->Unload();
        }
        m_subMeshes.clear();
    }

	void Mesh::MakeLine(Maths::Vector3 p_start, Maths::Vector3 p_end)
	{
		using namespace Graphics::Buffers;
		m_subMeshes.emplace_back(std::make_shared<SubMesh>(this));

		Vertex start;
		start.position	= { p_start.x, p_start.y, p_start.z };
		m_subMeshes[0]->m_loadedVertices.push_back(start);

		Vertex end;
		end.position = { p_end.x, p_end.y, p_end .z};
		m_subMeshes[0]->m_loadedVertices.push_back(end);	

		m_subMeshes[0]->m_loadedIndices.push_back(0);
		m_subMeshes[0]->m_loadedIndices.push_back(1);

		m_subMeshes[0]->m_vertexBuffer.reset(IVertexBuffer::Create(&m_subMeshes[0]->m_loadedVertices[0], sizeof(Vertex) * m_subMeshes[0]->m_loadedVertices.size()));
		m_subMeshes[0]->m_vertexArray.reset(IVertexArray::Create());
		m_subMeshes[0]->m_indexBuffer.reset(IIndexBuffer::Create(&m_subMeshes[0]->m_loadedIndices[0], static_cast<uint32_t>(m_subMeshes[0]->m_loadedIndices.size())));

		//Position
		m_subMeshes[0]->m_vertexBuffer->Push(3, CHAOS_FLOAT);
		//Normals
		m_subMeshes[0]->m_vertexBuffer->Push(3, CHAOS_FLOAT);
		//UV
		m_subMeshes[0]->m_vertexBuffer->Push(3, CHAOS_FLOAT);
		//COLOR
		m_subMeshes[0]->m_vertexBuffer->Push(4, CHAOS_FLOAT);

		m_subMeshes[0]->m_vertexArray->AddBuffer(*m_subMeshes[0]->m_vertexBuffer);
		m_subMeshes[0]->m_state.store(E_ResourceState::LOADED);
		m_state.store(E_ResourceState::LOADED);
	}
}