#include "..\..\include\ChaosPhysics\Shape\MeshShape.h"

Chaos::Physics::MeshShape::MeshShape(TriangleMeshInfo p_triangleMeshInfo) : m_meshInfo(p_triangleMeshInfo)
{

};


Chaos::Physics::MeshShape::~MeshShape()
{
}

void Chaos::Physics::MeshShape::CreateMeshShape()
{
}

Chaos::Physics::Shape * Chaos::Physics::MeshShape::CreateMeshShape(std::vector<Maths::Vector3> p_verticies, std::vector<Maths::Vector3> p_indicies, uint32_t p_numberOfTriangle, Friction p_friction, bool p_skipMeshCleanup, bool p_skipEdgeData)
{
	//Convert Maths::Vector3 -> physx::Pxvec3
	std::vector<physx::PxVec3> verticies;
	std::vector<physx::PxVec3> indicies;
	for (uint32_t i = 0; i < p_verticies.size(); ++i)
	{
		verticies.emplace_back(static_cast<physx::PxReal>(p_verticies[i].x), static_cast<physx::PxReal>(p_verticies[i].y), static_cast<physx::PxReal>(p_verticies[i].z));
	}
	for (uint32_t i = 0; i < p_indicies.size(); ++i)
	{
		indicies.emplace_back(static_cast<physx::PxReal>(p_indicies[i].x), static_cast<physx::PxReal>(p_indicies[i].y), static_cast<physx::PxReal>(p_indicies[i].z));
	}

	//Setting Up TriangleMeshDescription
	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<physx::PxU32>(verticies.size());
	meshDesc.points.data = &verticies[0];
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.triangles.count = static_cast<physx::PxU32>(p_numberOfTriangle);
	meshDesc.triangles.data = &indicies[0];
	meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);

	physx::PxCookingParams params = ElementCreator::Get().GetPxCooking()->getParams();

	params.suppressTriangleMeshRemapTable = true;

	//Setting Up Cooking Time vs Contact creating Time SkipMeshCleanUp/SkipEdgeData accelerate cooking process(with less precise mesh) but slows down run-time collision. 
	if (!p_skipMeshCleanup)
		params.meshPreprocessParams &= ~static_cast<physx::PxMeshPreprocessingFlags>(physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	else
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

	if (!p_skipEdgeData)
		params.meshPreprocessParams &= ~static_cast<physx::PxMeshPreprocessingFlags>(physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
	else
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

	ElementCreator::Get().GetPxCooking()->setParams(params);
	physx::PxTriangleMesh* triMesh = nullptr;

	//Save Mesh Data into PhysX Stream to use it later
	physx::PxDefaultMemoryOutputStream outBuffer;
	ElementCreator::Get().GetPxCooking()->cookTriangleMesh(meshDesc, outBuffer);

	physx::PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
	triMesh = ElementCreator::Get().GetPxPhysics()->createTriangleMesh(stream);

	//Create Shape
	Shape* toReturn = new Shape(ElementCreator::Get().GetPxPhysics()->createShape(physx::PxTriangleMeshGeometry(triMesh), *p_friction.GetMaterial(), true));
	toReturn->SetType(ColliderType::MESH);
	return toReturn;
}

void Chaos::Physics::MeshShape::CookMesh()
{
}

void Chaos::Physics::MeshShape::SetUpCookingParams(physx::PxCookingParams p_params, bool p_skipMeshCleanup, bool p_skipEdgeData)
{
	p_params.suppressTriangleMeshRemapTable = true;


	if (!p_skipMeshCleanup)
		p_params.meshPreprocessParams &= ~static_cast<physx::PxMeshPreprocessingFlags>(physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	else
		p_params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

	if (!p_skipEdgeData)
		p_params.meshPreprocessParams &= ~static_cast<physx::PxMeshPreprocessingFlags>(physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
	else
		p_params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
}