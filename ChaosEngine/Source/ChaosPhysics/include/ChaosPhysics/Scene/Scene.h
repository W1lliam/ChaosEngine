#pragma once
#include "ChaosPhysics/Actor/PhysicsActor.h"
#include "ChaosCore/Transform.h"

namespace Chaos
{
	namespace Physics
	{
		class Scene
		{
		public:
			Scene(const Maths::Vector3& p_gravity = Maths::Vector3(0.0f, -9.81f, 0.0f));
			~Scene() = default;
			
			void AddActor   (Actor& p_actor) const;
			void DeleteActor(Actor& p_actor) const;
			void AddBox     (ActorType p_type, Maths::Vector3 p_halfLength, Transform p_transform, Maths::Vector3 p_velocity, Friction p_friction, bool p_useRayCast, float p_mass = 0) const;
			void AddSphere  (ActorType p_type, float p_radius, Transform p_transform, Maths::Vector3 p_velocity, Friction p_friction, bool p_useRayCast, float p_mass = 0) const;
			void AddPlane   (Maths::Vector3 p_planeNormals, Friction p_friction, float p_distance) const;
			//getter
			physx::PxScene* GetScene() const;
            uint32_t GetActorCount() const;
            
			void SetGravity(const Maths::Vector3& p_gravity);

			void InitSceneVisualDebugger()          const;
			void DestroyScene()                     const;
			void SimulatePhysics(float p_deltaTime) const;


		private:
			physx::PxScene* m_scene = nullptr;
		};
	}
}
