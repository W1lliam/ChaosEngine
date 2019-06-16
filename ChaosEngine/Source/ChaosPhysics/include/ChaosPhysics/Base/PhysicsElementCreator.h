#pragma once
#include "Cooking.h"
#include "VisualDebugger.h"
#include <atomic>
#include <mutex>


namespace Chaos
{
    class Loader;

	namespace Physics
	{
		class ElementCreator
		{
		public:
			static ElementCreator& Get();
			physx::PxPhysics* GetPxPhysics() const;
			physx::PxDefaultCpuDispatcher* GetCpuDispatcher() const;
			physx::PxCooking* GetPxCooking() const;
			void DestroyPhysX() const;
			
			//Deleted constructors
			ElementCreator(ElementCreator const&) = delete;
			ElementCreator(ElementCreator&&) = delete;
			ElementCreator& operator=(ElementCreator const&) = delete;
			ElementCreator& operator=(ElementCreator&&) = delete;

		private:
            friend class ::Chaos::Loader;
			ElementCreator();
			~ElementCreator();
			void InitPhysX();
			void InitPhysics(physx::PxTolerancesScale& p_tolerance);
			void InitCpuThreads(int p_numberofThreadsForSimuation);


		private:
			physx::PxPhysics* m_physics = nullptr;
			physx::PxDefaultCpuDispatcher* m_cpuDispatcher = nullptr;
			Foundation m_foundation;
			VisualDebugger m_visualDebugger;
			Cooking	m_cooking;
		};
	}
}