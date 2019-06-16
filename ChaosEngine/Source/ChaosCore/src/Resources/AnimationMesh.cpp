#include "ChaosCore\Resources\AnimationMesh.h"
#include "ChaosCore/Resources/SkeletalMesh.h"
#include "ChaosCore/Resources/Managers/MaterialManager.h"
Chaos::Resources::AnimMesh::AnimMesh()
{
	
}
void Chaos::Resources::AnimMesh::Prepare(std::ifstream & p_stream)
{
	m_state.store(AResource::E_ResourceState::PREPARING);
	CHAOS_CORE_ASSERT(p_stream.is_open(), "Submesh can only load from an opened stream")
		AssetType type;
	p_stream.read(reinterpret_cast<char*>(&type), 1);
	if (type != AssetType::E_ASSETTYPE_ANIMATIONMESH)
	{
		CHAOS_CORE_ERROR("Asset type is different from expected type");
		p_stream.close();
		m_state.store(AResource::E_ResourceState::UNPREPARED);
		throw std::exception((std::string("AnimationMesh Asset") + " is not valid").c_str());
	}
	m_state.store(AResource::E_ResourceState::PREPARED);
}

void Chaos::Resources::AnimMesh::Load(std::ifstream & p_stream, SkeletalPtr& p_skeleton)
{
	m_state.store(AResource::E_ResourceState::LOADING);
	CHAOS_CORE_ASSERT(p_stream.is_open(), "Asset file has not been opened");

	uint32_t blockSize;
	p_stream.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));

	getline(p_stream, m_meshName, '\0');

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
		m_vertices.reserve(vertexCount);
		for (uint32_t v = 0; v < vertexCount; ++v)
		{
			WeightedVertex vertex;

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

				if (uv == 0)
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

			m_vertices.push_back(vertex);
		}
	}

	uint32_t faceCount = 0;
	p_stream.read(reinterpret_cast<char*>(&faceCount), sizeof(faceCount));
	m_indicies.reserve(faceCount * 3);

	for (uint32_t face = 0; face < faceCount; ++face)
	{
		for (uint8_t i = 0; i < 3; ++i)
		{
			uint32_t index;
			p_stream.read(reinterpret_cast<char*>(&index), sizeof(index));
			m_indicies.push_back(index);
		}
	}
	uint32_t boneCount = 0;
	p_stream.read(reinterpret_cast<char*>(&boneCount), sizeof(boneCount));
	for (uint32_t bone = 0; bone < boneCount; ++bone)
	{
		uint32_t vertexSize = 0;
		std::string boneName{};
		getline(p_stream, boneName, '\0');
		uint32_t id = p_skeleton->GetBoneByName(boneName)->GetBoneID();
		p_stream.read(reinterpret_cast<char*>(&vertexSize), sizeof(vertexSize));
		for (uint32_t i = 0; i < vertexSize; ++i)
		{	
			uint32_t WeightVertexIndex;
			float Weight;
			p_stream.read(reinterpret_cast<char*>(&WeightVertexIndex), sizeof(WeightVertexIndex));
			p_stream.read(reinterpret_cast<char*>(&Weight), sizeof(Weight));
			m_vertices[WeightVertexIndex].boneWeights.insert(Weight);
			m_vertices[WeightVertexIndex].boneIndices.insert(id);
		}
	}
	for (uint32_t i = 0; i < m_vertices.size(); ++i)
	{
		m_vertices[i].ResetBoneIndiciesNegativity();
		m_vertices[i].ResetBoneWeightsNegativity();
	}
}

void Chaos::Resources::AnimMesh::PostLoad()
{
	using namespace Graphics::Buffers;

	m_vertexBuffer.reset(IVertexBuffer::Create(&m_vertices[0], sizeof(WeightedVertex) * m_vertices.size()));
	m_vertexArray.reset(IVertexArray::Create());
	m_indexBuffer.reset(IIndexBuffer::Create(&m_indicies[0], static_cast<uint32_t>(m_indicies.size())));

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
	//BoneIndices
	m_vertexBuffer->Push(4, CHAOS_INT);
	//BoneWeight
	m_vertexBuffer->Push(4, CHAOS_FLOAT);
	//m_vertexBuffer->Push(4)
	m_vertexArray->AddBuffer(*m_vertexBuffer);
	m_state.store(AResource::E_ResourceState::LOADED);
}

void Chaos::Resources::AnimMesh::Unload()
{
	m_state.store(AResource::E_ResourceState::UNLOADING);
	m_vertices.clear();
	m_indicies.clear();

	m_vertexArray.reset();
	m_vertexBuffer.reset();
	m_indexBuffer.reset();
	m_state.store(AResource::E_ResourceState::UNPREPARED);
}