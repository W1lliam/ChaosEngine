#include "ChaosPhysics/Query/GeometryQuery.h"
//RAYCAST

bool Chaos::Physics::GeometryQuery::RayCast::FromAnActor(Physics::Actor* p_actor, Maths::Vector3 p_destination,
	float p_distance, RayCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
{
	if (!p_actor)
		return false;
	bool flag = false;
	//calculate where to raycast
	physx::PxVec3 origin = p_actor->GetPhysicsActor().getGlobalPose().p;
	physx::PxVec3 destination = Converter::ConvertVector3ToPxVec3(p_destination.Normalized());

	//get the scene 
	physx::PxScene* owner = p_actor->GetPhysicsActor().getScene();
	if (!owner)
		return false;
	//setup the flag
	const physx::PxHitFlags hitflag = physx::PxHitFlag::eDEFAULT;
	
	//Get the Reference of original actor's collisionMask
	physx::PxFilterData filter;
	filter.word0 = CollisionMask::SELF;
	physx::PxShape* ActorShape;
	p_actor->GetPhysicsActor().getShapes(&ActorShape, 1);
	physx::PxFilterData reference = ActorShape->getQueryFilterData();

	//Replace actor's collisionMask to Self to Avoid raycasting to itself
	ActorShape->setQueryFilterData(filter);
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}
	else
	{
		filterData.data.word0 = CollisionMask::C_DYNAMIC | CollisionMask::C_KINETIC | CollisionMask::C_STATIC;
	}
	if(p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxRaycastHit hitBuffer[bufferSize];
		physx::PxRaycastBuffer buf(hitBuffer, bufferSize);
		owner->lockWrite();
		if(owner->raycast(origin, destination, p_distance, buf, hitflag, filterData))
		{
			for(physx::PxU32 i = 0; i < bufferSize; ++i)
			{
				if(buf.getAnyHit(i).shape)
				{
					p_castInfo.rayCast.push_back(buf.getAnyHit(i));
					++p_castInfo.numberOfHits;
				}
				else
				{
					break;
				}
			}
			flag = true;
		}
		owner->unlockWrite();
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxRaycastHit hitBuffer[bufferSize];
		physx::PxRaycastBuffer buf(hitBuffer, bufferSize);
		owner->lockWrite();
		if (owner->raycast(origin, destination, p_distance, buf, hitflag, filterData))
		{
			if (buf.getAnyHit(0).shape)
			{
				p_castInfo.rayCast.push_back(buf.getAnyHit(0));
				++p_castInfo.numberOfHits;
			}
			flag = true;
		}
		owner->unlockWrite();
	}
	ActorShape->setQueryFilterData(reference);
	return flag;
}

bool Chaos::Physics::GeometryQuery::RayCast::FromAPosition(Physics::Scene* p_scene, Maths::Vector3 p_startingPosition,
	Maths::Vector3 p_destination, float p_distance, RayCastInfo& p_castInfo, bool p_getMultipleData,
	CollisionMask p_toTestAgainst)
{
	if (!p_scene)
		return false;
	bool flag = false;
	//calculate where to raycast
	physx::PxVec3 origin = Converter::ConvertVector3ToPxVec3(p_startingPosition);
	physx::PxVec3 destination = Converter::ConvertVector3ToPxVec3(p_destination.Normalized());

	//get the scene 

	//setup the flag
	const physx::PxHitFlags hitflag = physx::PxHitFlag::eDEFAULT;
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}

	if (p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxRaycastHit hitBuffer[bufferSize];
		physx::PxRaycastBuffer buf(hitBuffer, bufferSize);
		p_scene->GetScene()->lockWrite();
		if (p_scene->GetScene()->raycast(origin, destination, p_distance, buf, hitflag, filterData))
		{
			for (physx::PxU32 i = 0; i < bufferSize; ++i)
			{
				if (buf.getAnyHit(i).shape)
				{
					p_castInfo.rayCast.push_back(buf.getAnyHit(i));
					++p_castInfo.numberOfHits;
				}
				else
				{
					break;
				}
			}
			flag = true;
		}
		p_scene->GetScene()->unlockWrite();
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxRaycastHit hitBuffer[bufferSize];
		physx::PxRaycastBuffer buf(hitBuffer, bufferSize);
		p_scene->GetScene()->lockWrite();
		if (p_scene->GetScene()->raycast(origin, destination, p_distance, buf, hitflag, filterData))
		{
			if (buf.getAnyHit(0).shape)
			{
				p_castInfo.rayCast.push_back(buf.getAnyHit(0));
				++p_castInfo.numberOfHits;
			}
			flag = true;
		}
		p_scene->GetScene()->unlockWrite();
	}
	return flag;
}

bool Chaos::Physics::GeometryQuery::GeometryCast::CapsuleFromAnActor(Physics::Actor* p_actor,
	Maths::Vector3 p_destination, float p_distance, float p_radius, float p_height, GeometryCastInfo& p_castInfo,
	bool p_getMultipleData, CollisionMask p_toTestAgainst)
{
	if (!p_actor)
		return false;

	bool flag = false;
	//calculate where to GeometryCast
	physx::PxVec3 destination = Converter::ConvertVector3ToPxVec3(p_destination.Normalized());

	//get the scene 
	physx::PxScene* owner = p_actor->GetPhysicsActor().getScene();
	if (!owner)
		return false;
	//setup the flag
	const physx::PxHitFlags hitflag = physx::PxHitFlag::eDEFAULT;

	//Get the Reference of original actor's collisionMask
	physx::PxFilterData filter;
	filter.word0 = CollisionMask::SELF;
	physx::PxShape* ActorShape;
	p_actor->GetPhysicsActor().getShapes(&ActorShape, 1);
	physx::PxFilterData reference = ActorShape->getQueryFilterData();

	//Replace actor's collisionMask to Self to Avoid raycasting to itself
	ActorShape->setQueryFilterData(filter);
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}
	else
	{
		filterData.data.word0 = CollisionMask::C_DYNAMIC | CollisionMask::C_KINETIC | CollisionMask::C_STATIC;
	}

	if (p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxSweepHit hitBuffer[bufferSize];
		physx::PxSweepBuffer buf(hitBuffer, bufferSize);
		if (p_height == 0)
		{
			owner->lockWrite();
			if (owner->sweep(physx::PxSphereGeometry(p_radius), p_actor->GetPhysicsActor().getGlobalPose(), destination, p_distance, buf, hitflag, filterData))
			{
				for (physx::PxU32 i = 0; i < bufferSize; ++i)
				{
					if (buf.getAnyHit(i).shape)
					{
						p_castInfo.geometryCast.push_back(buf.getAnyHit(i));
						++p_castInfo.numberOfHits;
					}
					else
					{
						break;
					}
				}
				flag = true;
			}
		}
		else
		{
			owner->lockWrite();
			if (owner->sweep(physx::PxCapsuleGeometry(p_radius, p_height), p_actor->GetPhysicsActor().getGlobalPose(), destination, p_distance, buf, hitflag, filterData))
			{
				for (physx::PxU32 i = 0; i < bufferSize; ++i)
				{
					if (buf.getAnyHit(i).shape)
					{
						p_castInfo.geometryCast.push_back(buf.getAnyHit(i));
						++p_castInfo.numberOfHits;
					}
					else
					{
						break;
					}
				}
				flag = true;
			}
		}
		owner->unlockWrite();
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxSweepHit hitBuffer[bufferSize];
		physx::PxSweepBuffer buf(hitBuffer, bufferSize);
		if (p_height == 0)
		{
			owner->lockWrite();
			if (owner->sweep(physx::PxSphereGeometry(p_radius), p_actor->GetPhysicsActor().getGlobalPose(), destination, p_distance, buf, hitflag, filterData))
			{
				if (buf.getAnyHit(0).shape)
				{
					p_castInfo.geometryCast.push_back(buf.getAnyHit(0));
					++p_castInfo.numberOfHits;
				}
				flag = true;
			}
		}
		else
		{
			owner->lockWrite();
			if (owner->sweep(physx::PxCapsuleGeometry(p_radius, p_height), p_actor->GetPhysicsActor().getGlobalPose(), destination, p_distance, buf, hitflag, filterData))
			{
				if (buf.getAnyHit(0).shape)
				{
					p_castInfo.geometryCast.push_back(buf.getAnyHit(0));
					++p_castInfo.numberOfHits;
				}
				flag = true;
			}
		}
		owner->unlockWrite();
	}
	ActorShape->setQueryFilterData(reference);
	return flag;
}

bool Chaos::Physics::GeometryQuery::GeometryCast::CapsuleFromAPosition(Physics::Scene* p_scene,
	Maths::Vector3 p_startingPosition, Maths::Quaternion p_startingRotation, Maths::Vector3 p_destination,
	float p_distance, float p_radius, float p_height, GeometryCastInfo& p_castInfo, bool p_getMultipleData,
	CollisionMask p_toTestAgainst)
{
	if (!p_scene)
		return false;

	bool flag = false;
	//calculate where to GeometryCast
	physx::PxVec3 destination = Converter::ConvertVector3ToPxVec3(p_destination.Normalized());
	physx::PxTransform startingPose = physx::PxTransform(Converter::ConvertVector3ToPxVec3(p_startingPosition), Converter::ConvertQuaternionToPxQuat(p_startingRotation));

	//setup the flag
	const physx::PxHitFlags hitflag = physx::PxHitFlag::eDEFAULT;
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}
	if (p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxSweepHit hitBuffer[bufferSize];
		physx::PxSweepBuffer buf(hitBuffer, bufferSize);
		if (p_height == 0)
		{
			p_scene->GetScene()->lockWrite();
			if (p_scene->GetScene()->sweep(physx::PxSphereGeometry(p_radius), startingPose, destination, p_distance, buf, hitflag, filterData))
			{
				for (physx::PxU32 i = 0; i < bufferSize; ++i)
				{
					if (buf.getAnyHit(i).shape)
					{
						p_castInfo.geometryCast.push_back(buf.getAnyHit(i));
						++p_castInfo.numberOfHits;
					}
					else
					{
						break;
					}
				}
				flag = true;
			}
		}
		else
		{
			p_scene->GetScene()->lockWrite();
			if (p_scene->GetScene()->sweep(physx::PxCapsuleGeometry(p_radius, p_height), startingPose, destination, p_distance, buf, hitflag, filterData))
			{
				for (physx::PxU32 i = 0; i < bufferSize; ++i)
				{
					if (buf.getAnyHit(i).shape)
					{
						p_castInfo.geometryCast.push_back(buf.getAnyHit(i));
						++p_castInfo.numberOfHits;
					}
					else
					{
						break;
					}
				}
				flag = true;
			}
		}
		p_scene->GetScene()->unlockWrite();
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxSweepHit hitBuffer[bufferSize];
		physx::PxSweepBuffer buf(hitBuffer, bufferSize);
		if (p_height == 0)
		{
			p_scene->GetScene()->lockWrite();
			if (p_scene->GetScene()->sweep(physx::PxSphereGeometry(p_radius), startingPose, destination, p_distance, buf, hitflag, filterData))
			{
				if (buf.getAnyHit(0).shape)
				{
					p_castInfo.geometryCast.push_back(buf.getAnyHit(0));
					++p_castInfo.numberOfHits;
				}
				flag = true;
			}
		}
		else
		{
			p_scene->GetScene()->lockWrite();
			if (p_scene->GetScene()->sweep(physx::PxCapsuleGeometry(p_radius, p_height), startingPose, destination, p_distance, buf, hitflag, filterData))
			{
				if (buf.getAnyHit(0).shape)
				{
					p_castInfo.geometryCast.push_back(buf.getAnyHit(0));
					++p_castInfo.numberOfHits;
				}
				flag = true;
			}
		}
		p_scene->GetScene()->unlockWrite();
	}
	return flag;
}

//BOXCAST
bool Chaos::Physics::GeometryQuery::GeometryCast::BoxFromAnActor(Physics::Actor* p_actor, Maths::Vector3 p_destination,
	float p_distance, Maths::Vector3 p_halfLength, GeometryCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
{
	if (!p_actor)
		return false;
	bool flag = false;
	//calculate where to GeometryCast
	physx::PxVec3 destination = Converter::ConvertVector3ToPxVec3(p_destination.Normalized());

	//get the scene 
	physx::PxScene* owner = p_actor->GetPhysicsActor().getScene();
	if (!owner)
		return false;
	//setup the flag
	const physx::PxHitFlags hitflag = physx::PxHitFlag::eDEFAULT;

	//Get the Reference of original actor's collisionMask
	physx::PxFilterData filter;
	filter.word0 = CollisionMask::SELF;
	physx::PxShape* ActorShape;
	p_actor->GetPhysicsActor().getShapes(&ActorShape, 1);
	physx::PxFilterData reference = ActorShape->getQueryFilterData();

	//Replace actor's collisionMask to Self to Avoid raycasting to itself
	ActorShape->setQueryFilterData(filter);
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}
	else
	{
		filterData.data.word0 = CollisionMask::C_DYNAMIC | CollisionMask::C_KINETIC | CollisionMask::C_STATIC;
	}

	if (p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxSweepHit hitBuffer[bufferSize];
		physx::PxSweepBuffer buf(hitBuffer, bufferSize);
		owner->lockWrite();
		if (owner->sweep(physx::PxBoxGeometry(p_halfLength.x, p_halfLength.y, p_halfLength.z), p_actor->GetPhysicsActor().getGlobalPose(), destination, p_distance, buf, hitflag, filterData))
		{
			for (physx::PxU32 i = 0; i < bufferSize; ++i)
			{
				if (buf.getAnyHit(i).shape)
				{
					p_castInfo.geometryCast.push_back(buf.getAnyHit(i));
					++p_castInfo.numberOfHits;
				}
				else
				{
					break;
				}
			}
			flag = true;
		}
		owner->unlockWrite();
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxSweepHit hitBuffer[bufferSize];
		physx::PxSweepBuffer buf(hitBuffer, bufferSize);
		owner->lockWrite();
		if (owner->sweep(physx::PxBoxGeometry(p_halfLength.x, p_halfLength.y, p_halfLength.z), p_actor->GetPhysicsActor().getGlobalPose(), destination, p_distance, buf, hitflag, filterData))
		{
			if (buf.getAnyHit(0).shape)
			{
				p_castInfo.geometryCast.push_back(buf.getAnyHit(0));
				++p_castInfo.numberOfHits;
			}
			flag = true;
		}
		owner->unlockWrite();
	}
	ActorShape->setQueryFilterData(reference);
	return flag;
}

bool Chaos::Physics::GeometryQuery::GeometryCast::BoxFromAPosition(Physics::Scene* p_scene,
	Maths::Vector3 p_startingPosition, Maths::Quaternion p_startingRotation, Maths::Vector3 p_destination, float p_distance,
	Maths::Vector3 p_halfLength, GeometryCastInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
{
	if (!p_scene)
		return false;

	bool flag = false;
	//calculate where to GeometryCast
	physx::PxVec3 destination = Converter::ConvertVector3ToPxVec3(p_destination.Normalized());
	physx::PxTransform startingPose = physx::PxTransform(Converter::ConvertVector3ToPxVec3(p_startingPosition), Converter::ConvertQuaternionToPxQuat(p_startingRotation));

	//setup the flag
	const physx::PxHitFlags hitflag = physx::PxHitFlag::eDEFAULT;
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}
	if (p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxSweepHit hitBuffer[bufferSize];
		physx::PxSweepBuffer buf(hitBuffer, bufferSize);
		p_scene->GetScene()->lockWrite();
		{
			if (p_scene->GetScene()->sweep(physx::PxBoxGeometry(p_halfLength.x, p_halfLength.y, p_halfLength.z), startingPose, destination, p_distance, buf, hitflag, filterData))
			{
				for (physx::PxU32 i = 0; i < bufferSize; ++i)
				{
					if (buf.getAnyHit(i).shape)
					{
						p_castInfo.geometryCast.push_back(buf.getAnyHit(i));
						++p_castInfo.numberOfHits;
					}
					else
					{
						break;
					}
				}
				flag = true;
			}
		}
		p_scene->GetScene()->unlockWrite();
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxSweepHit hitBuffer[bufferSize];
		physx::PxSweepBuffer buf(hitBuffer, bufferSize);
		p_scene->GetScene()->lockWrite();
		if (p_scene->GetScene()->sweep(physx::PxBoxGeometry(p_halfLength.x, p_halfLength.y, p_halfLength.z), startingPose, destination, p_distance, buf, hitflag, filterData))
		{
			if (buf.getAnyHit(0).shape)
			{
				p_castInfo.geometryCast.push_back(buf.getAnyHit(0));
				++p_castInfo.numberOfHits;
			}
			flag = true;
		}
		p_scene->GetScene()->unlockWrite();
	}
	return flag;
}


//OVERLAPS
bool Chaos::Physics::GeometryQuery::OverLap::CapsuleFromAnActor(Physics::Actor* p_actor, float p_radius, float p_height,
	OverLapInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
{
	if (!p_actor)
		return false;

	bool flag = false;
	//calculate where to GeometryCast
	//get the scene 
	physx::PxScene* owner = p_actor->GetPhysicsActor().getScene();
	if (!owner)
		return false;
	//Get the Reference of original actor's collisionMask
	physx::PxFilterData filter;
	filter.word0 = CollisionMask::SELF;
	physx::PxShape* ActorShape;
	p_actor->GetPhysicsActor().getShapes(&ActorShape, 1);
	physx::PxFilterData reference = ActorShape->getQueryFilterData();

	//Replace actor's collisionMask to Self to Avoid raycasting to itself
	ActorShape->setQueryFilterData(filter);
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}
	else
	{
		filterData.data.word0 = CollisionMask::C_DYNAMIC | CollisionMask::C_KINETIC | CollisionMask::C_STATIC;
	}


	if (p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxOverlapHit hitBuffer[bufferSize];
		physx::PxOverlapBuffer buf(hitBuffer, bufferSize);
		if (p_height == 0)
		{
			owner->lockWrite();
			if (owner->overlap(physx::PxSphereGeometry(p_radius), p_actor->GetPhysicsActor().getGlobalPose(), buf, filterData))
			{
				for (physx::PxU32 i = 0; i < bufferSize; ++i)
				{
					if (buf.getAnyHit(i).shape)
					{
						p_castInfo.overlap.push_back(buf.getAnyHit(i));
						++p_castInfo.numberOfHits;
					}
					else
					{
						break;
					}
				}
				flag = true;
			}
		}
		else
		{
			owner->lockWrite();
			if (owner->overlap(physx::PxCapsuleGeometry(p_radius, p_height), p_actor->GetPhysicsActor().getGlobalPose(), buf, filterData))
			{
				for (physx::PxU32 i = 0; i < bufferSize; ++i)
				{
					if (buf.getAnyHit(i).shape)
					{
						p_castInfo.overlap.push_back(buf.getAnyHit(i));
						++p_castInfo.numberOfHits;
					}
					else
					{
						break;
					}
				}
				flag = true;
			}
		}
		owner->unlockWrite();
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxOverlapHit hitBuffer[bufferSize];
		physx::PxOverlapBuffer buf(hitBuffer, bufferSize);
		if (p_height == 0)
		{
			owner->lockWrite();
			if (owner->overlap(physx::PxSphereGeometry(p_radius), p_actor->GetPhysicsActor().getGlobalPose(), buf, filterData))
			{
				if (buf.getAnyHit(0).shape)
				{
					p_castInfo.overlap.push_back(buf.getAnyHit(0));
					++p_castInfo.numberOfHits;
				}
				flag = true;
			}
		}
		else
		{
			owner->lockWrite();
			if (owner->overlap(physx::PxCapsuleGeometry(p_radius, p_height), p_actor->GetPhysicsActor().getGlobalPose(), buf, filterData))
			{
				if (buf.getAnyHit(0).shape)
				{
					p_castInfo.overlap.push_back(buf.getAnyHit(0));
					++p_castInfo.numberOfHits;
				}
				flag = true;
			}
		}
		owner->unlockWrite();
	}
	ActorShape->setQueryFilterData(reference);
	return flag;
}

bool Chaos::Physics::GeometryQuery::OverLap::CapsuleFromAPosition(Physics::Scene* p_scene,
	Maths::Vector3 p_testingPosition, Maths::Quaternion p_testingRotation, float p_radius, float p_height,
	OverLapInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
{
	if (!p_scene)
		return false;
	bool flag = false;
	//calculate where to GeometryCast
	//get the scene 
	physx::PxTransform startingPose = physx::PxTransform(Converter::ConvertVector3ToPxVec3(p_testingPosition), Converter::ConvertQuaternionToPxQuat(p_testingRotation));
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}
	if (p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxOverlapHit hitBuffer[bufferSize];
		physx::PxOverlapBuffer buf(hitBuffer, bufferSize);
		if (p_height == 0)
		{
			p_scene->GetScene()->lockWrite();
			if (p_scene->GetScene()->overlap(physx::PxSphereGeometry(p_radius), startingPose, buf, filterData))
			{
				for (physx::PxU32 i = 0; i < bufferSize; ++i)
				{
					if (buf.getAnyHit(i).shape)
					{
						p_castInfo.overlap.push_back(buf.getAnyHit(i));
						++p_castInfo.numberOfHits;
					}
					else
					{
						break;
					}
				}
				flag = true;
			}
		}
		else
		{
			p_scene->GetScene()->lockWrite();
			if (p_scene->GetScene()->overlap(physx::PxCapsuleGeometry(p_radius, p_height), startingPose, buf, filterData))
			{
				for (physx::PxU32 i = 0; i < bufferSize; ++i)
				{
					if (buf.getAnyHit(i).shape)
					{
						p_castInfo.overlap.push_back(buf.getAnyHit(i));
						++p_castInfo.numberOfHits;
					}
					else
					{
						break;
					}
				}
				flag = true;
			}
		}
		p_scene->GetScene()->unlockWrite();
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxOverlapHit hitBuffer[bufferSize];
		physx::PxOverlapBuffer buf(hitBuffer, bufferSize);
		if (p_height == 0)
		{
			p_scene->GetScene()->lockWrite();
			if (p_scene->GetScene()->overlap(physx::PxSphereGeometry(p_radius), startingPose, buf, filterData))
			{
				if (buf.getAnyHit(0).shape)
				{
					p_castInfo.overlap.push_back(buf.getAnyHit(0));
					++p_castInfo.numberOfHits;
				}
				flag = true;
			}
		}
		else
		{
			p_scene->GetScene()->lockWrite();
			if (p_scene->GetScene()->overlap(physx::PxCapsuleGeometry(p_radius, p_height), startingPose, buf, filterData))
			{
				if (buf.getAnyHit(0).shape)
				{
					p_castInfo.overlap.push_back(buf.getAnyHit(0));
					++p_castInfo.numberOfHits;
				}
				flag = true;
			}
		}
		p_scene->GetScene()->unlockWrite();
	}
	return flag;
}

bool Chaos::Physics::GeometryQuery::OverLap::BoxFromAnActor(Physics::Actor* p_actor, Maths::Vector3 p_halfLength,
	OverLapInfo& p_castInfo, bool p_getMultipleData, CollisionMask p_toTestAgainst)
{
	if (!p_actor)
		return false;

	bool flag = false;
	//calculate where to GeometryCast
	//get the scene 
	physx::PxScene* owner = p_actor->GetPhysicsActor().getScene();
	if (!owner)
		return false;
	//Get the Reference of original actor's collisionMask
	physx::PxFilterData filter;
	filter.word0 = CollisionMask::SELF;
	physx::PxShape* ActorShape;
	p_actor->GetPhysicsActor().getShapes(&ActorShape, 1);
	physx::PxFilterData reference = ActorShape->getQueryFilterData();

	//Replace actor's collisionMask to Self to Avoid raycasting to itself
	ActorShape->setQueryFilterData(filter);
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}
	else
	{
		filterData.data.word0 = CollisionMask::C_DYNAMIC | CollisionMask::C_KINETIC | CollisionMask::C_STATIC;
	}


	if (p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxOverlapHit hitBuffer[bufferSize];
		physx::PxOverlapBuffer buf(hitBuffer, bufferSize);

		owner->lockWrite();
		if (owner->overlap(physx::PxBoxGeometry(p_halfLength.x, p_halfLength.y, p_halfLength.z), p_actor->GetPhysicsActor().getGlobalPose(), buf, filterData))
		{
			for (physx::PxU32 i = 0; i < bufferSize; ++i)
			{
				if (buf.getAnyHit(i).shape)
				{
					p_castInfo.overlap.push_back(buf.getAnyHit(i));
					++p_castInfo.numberOfHits;
				}
				else
				{
					break;
				}
			}
			flag = true;
		}
		owner->unlockWrite();
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxOverlapHit hitBuffer[bufferSize];
		physx::PxOverlapBuffer buf(hitBuffer, bufferSize);
		owner->lockWrite();
		if (owner->overlap(physx::PxBoxGeometry(p_halfLength.x, p_halfLength.y, p_halfLength.z), p_actor->GetPhysicsActor().getGlobalPose(), buf, filterData))
		{
			if (buf.getAnyHit(0).shape)
			{
				p_castInfo.overlap.push_back(buf.getAnyHit(0));
				++p_castInfo.numberOfHits;
			}
			flag = true;
		}
		owner->unlockWrite();
	}
	ActorShape->setQueryFilterData(reference);
	return flag;
}

bool Chaos::Physics::GeometryQuery::OverLap::BoxFromAPosition(Physics::Scene* p_scene, Maths::Vector3 p_testingPosition,
	Maths::Quaternion p_testingRotation, Maths::Vector3 p_halfLength, OverLapInfo& p_castInfo, bool p_getMultipleData,
	CollisionMask p_toTestAgainst)
{
	if (!p_scene)
		return false;
	bool flag = false;
	//calculate where to GeometryCast
	//get the scene 
	physx::PxTransform startingPose = physx::PxTransform(Converter::ConvertVector3ToPxVec3(p_testingPosition), Converter::ConvertQuaternionToPxQuat(p_testingRotation));
	physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
	if (p_toTestAgainst != CollisionMask::C_EMPTY)
	{
		filterData.data.word0 = p_toTestAgainst;
	}
	if (p_getMultipleData)
	{
		const physx::PxU32 bufferSize = 256;
		physx::PxOverlapHit hitBuffer[bufferSize];
		physx::PxOverlapBuffer buf(hitBuffer, bufferSize);
		p_scene->GetScene()->lockWrite();
		if (p_scene->GetScene()->overlap(physx::PxBoxGeometry(p_halfLength.x, p_halfLength.y, p_halfLength.z), startingPose, buf, filterData))
		{
			for (physx::PxU32 i = 0; i < bufferSize; ++i)
			{
				if (buf.getAnyHit(i).shape)
				{
					p_castInfo.overlap.push_back(buf.getAnyHit(i));
					++p_castInfo.numberOfHits;
				}
				else
				{
					break;
				}
			}
			flag = true;
		}
	}
	else
	{
		const physx::PxU32 bufferSize = 1;
		physx::PxOverlapHit hitBuffer[bufferSize];
		physx::PxOverlapBuffer buf(hitBuffer, bufferSize);
		p_scene->GetScene()->lockWrite();
		if (p_scene->GetScene()->overlap(physx::PxBoxGeometry(p_halfLength.x, p_halfLength.y, p_halfLength.z), startingPose, buf, filterData))
		{
			if (buf.getAnyHit(0).shape)
			{
				p_castInfo.overlap.push_back(buf.getAnyHit(0));
				++p_castInfo.numberOfHits;
			}
		flag = true;
		}
	}
	p_scene->GetScene()->unlockWrite();
	return flag;
}