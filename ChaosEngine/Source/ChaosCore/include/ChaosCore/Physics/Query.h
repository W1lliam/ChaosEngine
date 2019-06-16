#pragma once
#include "ChaosPhysics/Query/GeometryQuery.h"
#include "ChaosCore/Scene/Scene.h"

namespace Chaos
{
	class RigidBodyComponent;

	namespace RayCast
	{
		//bool multipleData means, if it's true, it will return all the shape data that come in contact with Ray
		//if it's false, it will return the first object that come in contact
		bool FromARigidBodyComponent(RigidBodyComponent* p_component, Maths::Vector3 p_destination, float p_distance, Physics::RayCastInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
		bool FromPosition(Chaos::Scene* p_scene, Maths::Vector3 p_startingPosition, Maths::Vector3 p_destination, float p_distance, Physics::RayCastInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
	}
	namespace GeometryCast
	{
		bool CapsuleFromARigidBodyComponent(RigidBodyComponent* p_component, Maths::Vector3 p_destination, float p_distance, float p_radius, float p_height, Physics::GeometryCastInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
		bool Capsule(Chaos::Scene* p_scene, Maths::Vector3 p_startingPosition, Maths::Quaternion p_startingRotation, Maths::Vector3 p_destination, float p_distance, float p_radius, float p_height, Physics::GeometryCastInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
		bool BoxFromARigidBodyComponent(RigidBodyComponent* p_component, Maths::Vector3 p_destination, float p_distance, Maths::Vector3 p_halfLength, Physics::GeometryCastInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
		bool Box(Chaos::Scene* p_scene, Maths::Vector3 p_startingPosition, Maths::Quaternion p_startingRotation, Maths::Vector3 p_destination, float p_distance, Maths::Vector3 p_halfLength, Physics::GeometryCastInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
	}
	namespace OverLap
	{
		bool CapsuleFromARigidBodyComponent(RigidBodyComponent* p_component, float p_radius, float p_height, Physics::OverLapInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
		bool Capsule(Chaos::Scene* p_scene, Maths::Vector3 p_testingPosition, Maths::Quaternion p_testingRotation, float p_radius, float p_height, Physics::OverLapInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
	    bool BoxFromARigidBodyComponent(RigidBodyComponent* p_component, Maths::Vector3 p_halfLength, Physics::OverLapInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
		bool Box(Chaos::Scene* p_scene, Maths::Vector3 p_testingPosition, Maths::Quaternion p_testingRotation, Maths::Vector3 p_halfLength, Physics::OverLapInfo& p_castInfo, bool p_getMultipleData, Physics::CollisionMask p_toTestAgainst = Physics::CollisionMask::C_EMPTY);
	}
}