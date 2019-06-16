#pragma once
#include "ChaosCore/ECS/System/System.h"
#include "ChaosCore/Scene/Scene.h"
#include "ChaosCore/Events/EventListener.h"
#include "ChaosCore/Events/PhysicsSceneEvents.h"

namespace Chaos
{
	class Collider;
	class PhysicsSystem : public System
	{
        CHAOS_SYSTEM(PhysicsSystem);
	public:
        PhysicsSystem() : System(System::VERY_HIGH_SYSTEM_PRIORITY)
        {}
		~PhysicsSystem()    = default;
		void		FixedUpdate(double p_fixedDeltaTime) override;
		void		Update(double p_deltaTime) override;
		void		UpdateMask();
		void		AddCollider(Collider& p_collider);
		void		AddNewMask(std::string p_collisionMask);
		void		DeleteMask(std::string p_collisionMask);
		
		static System& Load(ReadStream & p_stream);

		std::vector<Collider*>& GetColliderList();
		std::vector<std::string>& GetCollisionMask();
	private:
		std::vector<std::string> m_collisionMaskList;
		std::vector<Collider*> m_colliderList;
		bool m_maskUpdated = false;
	};
}