#include "ChaosPhysics/Scene/Scene.h"
#include "ChaosPhysics/Maths/Conversion.h"

namespace Chaos::Physics
{
    Scene::Scene(const Maths::Vector3& p_gravity)
    {
        physx::PxSceneDesc sceneDesc(ElementCreator::Get().GetPxPhysics()->getTolerancesScale());
        sceneDesc.gravity = physx::PxVec3(p_gravity.x, p_gravity.y, p_gravity.z);
        sceneDesc.cpuDispatcher = ElementCreator::Get().GetCpuDispatcher();
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        m_scene = ElementCreator::Get().GetPxPhysics()->createScene(sceneDesc);
        InitSceneVisualDebugger();
    }

    void Scene::AddActor(Actor& p_actor) const
    {
        m_scene->addActor(p_actor.PhysicsActor());
    }

    void Scene::DeleteActor(Actor& p_actor) const
    {
        m_scene->removeActor(p_actor.PhysicsActor());
    }

    void Scene::AddBox(ActorType p_type, Maths::Vector3 p_halfLength, Transform p_transform,
        Maths::Vector3 p_velocity, Friction p_friction, bool p_useRayCast, float p_mass) const
    {
        if (p_type == ActorType::Dynamic)
        {

            physx::PxRigidDynamic* box = physx::PxCreateDynamic(*ElementCreator::Get().GetPxPhysics(), Converter::ConvertTransformToPxTransform(p_transform).getNormalized(), physx::PxBoxGeometry(Converter::ConvertVector3ToPxVec3(p_halfLength)), *p_friction.GetMaterial(), p_mass);
            box->setAngularDamping(0.5f);
            box->setLinearVelocity(Converter::ConvertVector3ToPxVec3(p_velocity));
            m_scene->addActor(*box);
        }
        else
        {
            physx::PxRigidStatic* box = physx::PxCreateStatic(*ElementCreator::Get().GetPxPhysics(), Converter::ConvertTransformToPxTransform(p_transform).getNormalized(), physx::PxBoxGeometry(Converter::ConvertVector3ToPxVec3(p_halfLength)), *p_friction.GetMaterial());
            m_scene->addActor(*box);
        }
    }

    void Scene::AddSphere(ActorType p_type, float p_radius, Transform p_transform,
        Maths::Vector3 p_velocity, Friction p_friction, bool p_useRayCast, float p_mass) const
    {
        if (p_type == ActorType::Dynamic)
        {
            physx::PxTransform temp = Converter::ConvertTransformToPxTransform(p_transform);
            Friction friction(0.5f, 0.6f, 0.7f);
            physx::PxRigidDynamic* sphere = physx::PxCreateDynamic(*ElementCreator::Get().GetPxPhysics(), temp, physx::PxSphereGeometry(4), *friction.GetMaterial(), 100);
            sphere->setAngularDamping(0.5f);
            sphere->setLinearVelocity(Converter::ConvertVector3ToPxVec3(p_velocity));
            m_scene->addActor(*sphere);
        }
        else
        {
            physx::PxRigidStatic* sphere = physx::PxCreateStatic(*ElementCreator::Get().GetPxPhysics(), Converter::ConvertTransformToPxTransform(p_transform), physx::PxSphereGeometry(p_radius), *p_friction.GetMaterial());
            m_scene->addActor(*sphere);
        }
    }

    void Scene::AddPlane(Maths::Vector3 p_planeNormals, Friction p_friction, float p_distance) const
    {
        physx::PxRigidStatic* plane = physx::PxCreatePlane(*ElementCreator::Get().GetPxPhysics(), physx::PxPlane(Converter::ConvertVector3ToPxVec3(p_planeNormals), p_distance), *p_friction.GetMaterial());
        m_scene->addActor(*plane);
    }

	physx::PxScene* Scene::GetScene() const
	{
		return m_scene;
	}

	void Scene::SetGravity(const Maths::Vector3& p_gravity)
    {
        m_scene->setGravity(physx::PxVec3(p_gravity.x, p_gravity.y, p_gravity.z));
    }

    uint32_t Scene::GetActorCount() const
    {
        return m_scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
    }

    void Scene::InitSceneVisualDebugger() const
    {
        physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
        if (pvdClient)
        {
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }
    }

    void Scene::DestroyScene() const
    {
		if(m_scene)
		{
			physx::PxU32 number = m_scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
			physx::PxActor** Actors = new physx::PxActor*[number];
			m_scene->removeActors(Actors, number);
            m_scene->release();
		}
    }

    void Scene::SimulatePhysics(float p_deltaTime) const
    {
        m_scene->simulate(p_deltaTime);
        m_scene->fetchResults(true);
    }
}