#pragma once
#include <ChaosMaths.h>
#include <vector>
#include <map>
#include "ChaosCore/Resources/AResource.h"
#include "ChaosCore/Resources/Vertex.h"
#include "ChaosCore/Resources/Buffers/IIndexBuffer.h"
#include "ChaosCore/Resources/Buffers/IVertexArray.h"
#include "ChaosCore/Resources/Buffers/IVertexBuffer.h"
#include "ChaosCore/Resources/Material.h"
#include "ChaosCore/Resources/Skeleton.h"
#include "ChaosCore/Utils/StringID.h"
namespace Chaos::Resources
{
	class AnimMesh
	{
	friend class SkeletalMesh;

	public:
		AnimMesh();
		~AnimMesh() = default;

		inline const  std::string&					GetAnimMeshName()	const  { return m_meshName; };
		inline const  EntityID&						GetOwnerID()        const  { return m_owningEntityID; };
		inline std::vector<WeightedVertex>&			GetVertices()			   { return m_vertices; };
		inline std::vector<uint32_t>&				GetIndicies()		       { return m_indicies; };
		inline void									SetOwnerID(EntityID& p_id) { m_owningEntityID = p_id; };
		
		Graphics::Buffers::IVertexBuffer&   GetVertexBuffer()   const { return *m_vertexBuffer; }
		Graphics::Buffers::IIndexBuffer&    GetIndexBuffer()    const { return *m_indexBuffer; }
		Graphics::Buffers::IVertexArray&    GetVertexArray()    const { return *m_vertexArray; }

		bool                                UseVertexBuffer()   const { return m_vertexBuffer != nullptr; }
		bool                                UseIndexBuffer()    const { return m_indexBuffer != nullptr; }
		bool                                UseVertexArray()    const { return m_vertexArray != nullptr; }

		uint32_t                            GetVertexCount()    const { return static_cast<uint32_t>(m_vertices.size()); }
		AResource::E_ResourceState          GetState()          const { return m_state.load(); }

	protected:
		void Prepare(std::ifstream& p_stream);
		void Load(std::ifstream& p_stream, SkeletalPtr& p_skeleton);
		void PostLoad();
		void Unload();

	private:
		EntityID													m_owningEntityID;
		std::string													m_meshName{};
		std::vector<WeightedVertex>									m_vertices{};
		std::vector<uint32_t>										m_indicies{};
		std::atomic<AResource::E_ResourceState>						m_state{ AResource::E_ResourceState::UNPREPARED };
		
		std::unique_ptr<Graphics::Buffers::IVertexArray>    m_vertexArray;
		std::unique_ptr<Graphics::Buffers::IVertexBuffer>   m_vertexBuffer;
		std::unique_ptr<Graphics::Buffers::IIndexBuffer>    m_indexBuffer;
	};
	using AnimMeshPtr = std::shared_ptr<AnimMesh>;
}