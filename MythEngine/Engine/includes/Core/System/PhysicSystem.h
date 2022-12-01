#pragma once

#include <PhysX/PxPhysicsAPI.h>
#include "Core/System/EventManager.h"
#include "Core/GameObject.h"
#include "Core/BaseSingleton.h"

namespace Core
{
	namespace System
	{
		class PhysicSystem : public Core::BaseSingleton<PhysicSystem>
		{
		public:
			PhysicSystem();
			~PhysicSystem();

			static physx::PxFilterFlags FilterShader(
				physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
				physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
				physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

			void RemoveActor(physx::PxActor* p_actor);
			void Update();

			bool Raycast(const Vec3& p_from, const Vec3& p_dir, float p_maxDistance, physx::PxRaycastBuffer& p_hit);

			physx::PxDefaultErrorCallback	m_defaultErrorCallback;
			physx::PxDefaultAllocator		m_defaultAllocatorCallback;

			physx::PxPhysics* m_physics = nullptr;
			physx::PxScene* m_scene = nullptr;

		private:
			void ConnectPvd();
			void InitPhysicScene();

			physx::PxDefaultCpuDispatcher* m_dispatcher = nullptr;
			physx::PxFoundation* m_foundation = nullptr;
			physx::PxPvd* m_pvd;
			physx::PxPvdSceneClient* pvdClient;
			physx::PxPvdTransport* m_transport;
		};
	};
};