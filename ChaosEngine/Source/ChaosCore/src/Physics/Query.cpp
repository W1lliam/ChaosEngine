#include "ChaosCore/Physics/Query.h"
#include "ChaosCore/Components/RigidBodyComponent.h"

namespace Chaos
{
    using namespace Physics;
    bool RayCast::FromARigidBodyComponent(RigidBodyComponent* p_component, Maths::Vector3 p_destination,
        float p_distance, RayCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
    {

        return GeometryQuery::RayCast::FromAnActor(&p_component->Actor(), p_destination, p_distance, p_castInfo, p_getMultipleData, p_toTestAgainst);
    }

    bool RayCast::FromPosition(Chaos::Scene* p_scene, Maths::Vector3 p_startingPosition,
        Maths::Vector3 p_destination, float p_distance, RayCastInfo& p_castInfo, bool p_getMultipleData,
        CollisionMask p_toTestAgainst)
    {
        return GeometryQuery::RayCast::FromAPosition(&p_scene->GetPhysicsScene(), p_startingPosition, p_destination, p_distance, p_castInfo, p_getMultipleData, p_toTestAgainst);
    }

    bool GeometryCast::CapsuleFromARigidBodyComponent(RigidBodyComponent* p_component, Maths::Vector3 p_destination,
        float p_distance, float p_radius, float p_height, GeometryCastInfo& p_castInfo, bool p_getMultipleData,
        CollisionMask p_toTestAgainst)
    {
        return GeometryQuery::GeometryCast::CapsuleFromAnActor(&p_component->Actor(), p_destination, p_distance, p_radius, p_height,
            p_castInfo, p_getMultipleData, p_toTestAgainst);
    }

    bool GeometryCast::Capsule(Chaos::Scene* p_scene, Maths::Vector3 p_startingPosition,
        Maths::Quaternion p_startingRotation, Maths::Vector3 p_destination, float p_distance, float p_radius,
        float p_height, GeometryCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
    {
        return GeometryQuery::GeometryCast::CapsuleFromAPosition(&p_scene->GetPhysicsScene(), p_startingPosition, p_startingRotation, p_destination,
            p_distance, p_radius, p_height, p_castInfo, p_getMultipleData, p_toTestAgainst);
    }

    bool GeometryCast::BoxFromARigidBodyComponent(RigidBodyComponent* p_component, Maths::Vector3 p_destination,
        float p_distance, Maths::Vector3 p_halfLength, GeometryCastInfo& p_castInfo, bool p_getMultipleData,
        CollisionMask p_toTestAgainst)
    {
        return GeometryQuery::GeometryCast::BoxFromAnActor(&p_component->Actor(), p_destination, p_distance, p_halfLength, p_castInfo, p_getMultipleData, p_toTestAgainst);
    }

    bool GeometryCast::Box(Chaos::Scene* p_scene, Maths::Vector3 p_startingPosition,
        Maths::Quaternion p_startingRotation, Maths::Vector3 p_destination, float p_distance, Maths::Vector3 p_halfLength,
        GeometryCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
    {
        return GeometryQuery::GeometryCast::BoxFromAPosition(&p_scene->GetPhysicsScene(), p_startingPosition, p_startingRotation, p_destination, p_distance,
            p_halfLength, p_castInfo, p_getMultipleData, p_toTestAgainst);
    }

    bool OverLap::CapsuleFromARigidBodyComponent(RigidBodyComponent* p_component, float p_radius, float p_height,
        OverLapInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
    {
        return GeometryQuery::OverLap::CapsuleFromAnActor(&p_component->Actor(), p_radius, p_height, p_castInfo, p_getMultipleData, p_toTestAgainst);
    }

    bool OverLap::Capsule(Chaos::Scene* p_scene, Maths::Vector3 p_testingPosition,
        Maths::Quaternion p_testingRotation, float p_radius, float p_height, OverLapInfo& p_castInfo,
        bool p_getMultipleData, CollisionMask p_toTestAgainst)
    {
        return GeometryQuery::OverLap::CapsuleFromAPosition(&p_scene->GetPhysicsScene(), p_testingPosition, p_testingRotation, p_radius, p_height,
            p_castInfo, p_getMultipleData, p_toTestAgainst);
    }
    /*
    bool OverLap::BoxFromARigidBodyComponent(RigidBodyComponent* p_component, Maths::Vector3 p_halfLength,
        OverLapInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
    {
        return GeometryQuery::OverLap::BoxFromAnActor(p_component->GetActor(), p_halfLength, p_castInfo, p_getMultipleData, p_toTestAgainst);
    }*/
    /*
    bool OverLap::BoxFromAPosition(PhysicsScene* p_physicsScene, Maths::Vector3 p_testingPosition,
        Maths::Quaternion p_testingRotation, Maths::Vector3 p_halfLength, OverLapInfo& p_castInfo, bool p_getMultipleData,
        CollisionMask p_toTestAgainst)
    {
        return GeometryQuery::OverLap::BoxFromAPosition(p_physicsScene->GetScene(), p_testingPosition, p_testingRotation,
                                                        p_halfLength, p_castInfo, p_getMultipleData, p_toTestAgainst);
    }*/
}