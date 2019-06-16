#pragma once
#include "Shape.h"

namespace Chaos
{
	namespace Physics
	{
		struct TriangleMeshInfo
		{
			std::vector<physx::PxU32> verticies;
			std::vector<physx::PxU32> indices;
			uint32_t numberOfTriangle = 0;
		};

		class MeshShape : public Shape
		{
		public:
			MeshShape() = default;
			MeshShape(TriangleMeshInfo p_triangleMeshInfo);
			~MeshShape();
			void CreateMeshShape();
			static Shape* CreateMeshShape(std::vector<Maths::Vector3> p_verticies, std::vector<Maths::Vector3> p_indicies, uint32_t p_numberOfTriangle, Friction p_friction, bool p_skipMeshCleanup, bool p_skipEdgeData);
		private:
			void CookMesh();
			void SetUpCookingParams(physx::PxCookingParams p_params, bool p_skipMeshCleanup, bool p_skipEdgeData);
			TriangleMeshInfo m_meshInfo;
			physx::PxTriangleMesh* m_meshCollider = nullptr;
		};
	}
}
