#pragma once
#include "ChaosPhysics/Base/PhysicsElementCreator.h"
#include "ChaosPhysics/Scene/Scene.h"
#include "ChaosPhysics/Collision/CollisionMask.h"
#include <vector>

namespace Chaos
{
	namespace Physics
	{
		enum CollisionResponse
		{
			C_IGNORE,
			C_OVERLAP,
			C_BLOCK,
			C_TOUCH
		};

		struct RayCastInfo
		{
			std::vector<physx::PxRaycastHit> rayCast;
			uint32_t numberOfHits = 0;
		};

		struct GeometryCastInfo
		{
			std::vector<physx::PxSweepHit> geometryCast;
			uint32_t numberOfHits = 0;
		};

		struct OverLapInfo
		{
			std::vector<physx::PxOverlapHit> overlap;
			uint32_t numberOfHits = 0;
		};

		namespace GeometryQuery
		{
			namespace RayCast
			{
				//bool multipleData means, if it's true, it will return all the shape data that come in contact with Ray
				//if it's false, it will return the first object that come in contact
				 bool FromAnActor(Actor* p_actor, Maths::Vector3 p_destination, float p_distance, RayCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
				 bool FromAPosition(Scene* p_scene, Maths::Vector3 p_startingPosition, Maths::Vector3 p_destination, float p_distance, RayCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
			}
			namespace GeometryCast
			{
				bool CapsuleFromAnActor(Physics::Actor* p_actor, Maths::Vector3 p_destination, float p_distance, float p_radius, float p_height, GeometryCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
				bool CapsuleFromAPosition(Physics::Scene* p_scene, Maths::Vector3 p_startingPosition, Maths::Quaternion p_startingRotation, Maths::Vector3 p_destination, float p_distance, float p_radius, float p_height, GeometryCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
				bool BoxFromAnActor(Physics::Actor* p_actor, Maths::Vector3 p_destination, float p_distance, Maths::Vector3 p_halfLength, GeometryCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
				bool BoxFromAPosition(Physics::Scene* p_scene, Maths::Vector3 p_startingPosition, Maths::Quaternion p_startingRotation, Maths::Vector3 p_destination, float p_distance, Maths::Vector3 p_halfLength, GeometryCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
			}
			namespace OverLap
			{
				bool CapsuleFromAnActor(Physics::Actor* p_actor, float p_radius, float p_height, OverLapInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
				bool CapsuleFromAPosition(Physics::Scene* p_scene, Maths::Vector3 p_testingPosition, Maths::Quaternion p_testingRotation, float p_radius, float p_height, OverLapInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
				bool BoxFromAnActor(Physics::Actor* p_actor, Maths::Vector3 p_halfLength, OverLapInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
				bool BoxFromAPosition(Physics::Scene* p_scene, Maths::Vector3 p_testingPosition, Maths::Quaternion p_testingRotation, Maths::Vector3 p_halfLength, OverLapInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst = CollisionMask::C_EMPTY);
			}
		}
	}
}