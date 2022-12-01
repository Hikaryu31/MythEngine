#include "Core/System/PhysicSystem.h"
#include "Core/System/Time.h"
#include "Core/GameObject.h"

SINGLETON_SETUP(Core::System::PhysicSystem);

using namespace physx;

Core::System::PhysicSystem::PhysicSystem()
{
	MYTH_ASSERT(singleton == nullptr);
	singleton = this;

	//Fetch PVD connection port

	ConnectPvd();

	//PhysX scene init

	InitPhysicScene();

	//PVD setup

	pvdClient = m_scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}

Core::System::PhysicSystem::~PhysicSystem()
{	
	delete(m_scene->getSimulationEventCallback());
	m_scene->release();
	m_scene = nullptr;
	m_dispatcher->release();
	m_physics->release();
	m_pvd->disconnect();
	m_pvd->release();
	m_transport->release();
	m_foundation->release();
	singleton = nullptr;
}

/////////////////////////////////////////////////////  Public methods  /////////////////////////////////////////////////////

PxFilterFlags Core::System::PhysicSystem::FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_CONTACT_POINTS
		| PxPairFlag::eNOTIFY_TOUCH_LOST
		| PxPairFlag::eTRIGGER_DEFAULT;

	return PxFilterFlag::eDEFAULT;
}

void Core::System::PhysicSystem::RemoveActor(PxActor* p_actor)
{
	m_scene->removeActor(*p_actor, false);
	p_actor = nullptr;
}

void Core::System::PhysicSystem::Update()
{
	m_scene->simulate(1.f/60.f);
	m_scene->fetchResults(true);
	m_scene->checkResults();
	m_scene->flushSimulation();

	PxU32 nbActiveActors;
	PxActor** activeActors = m_scene->getActiveActors(nbActiveActors);

	for (PxU32 i = 0; i < nbActiveActors; ++i)
	{
		GameObject* renderObject = static_cast<GameObject*>(activeActors[i]->userData);

		if (renderObject)
			renderObject->setTransform(((PxRigidActor*)activeActors[i])->getGlobalPose());
	}
}

bool Core::System::PhysicSystem::Raycast(const Vec3& p_from, const Vec3& p_dir, float p_maxDistance, physx::PxRaycastBuffer& p_hit)
{
	return m_scene->raycast({ p_from.m_x,p_from.m_y,p_from.m_z }, { p_dir.m_x,p_dir.m_y,p_dir.m_z }, p_maxDistance, p_hit);
}

/////////////////////////////////////////////////////  Private methods  /////////////////////////////////////////////////////

void Core::System::PhysicSystem::ConnectPvd()
{
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION,
									  m_defaultAllocatorCallback,
									  m_defaultErrorCallback);

	m_pvd = PxCreatePvd(*m_foundation);
	m_transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_pvd->connect(*m_transport, PxPvdInstrumentationFlag::eALL);


	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation,
		PxTolerancesScale(), true, m_pvd);

	if (!m_physics) throw("PxCreatePhysics failed!");
}

void Core::System::PhysicSystem::InitPhysicScene()
{
	PxSceneDesc sceneDesc = PxSceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
	m_dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_dispatcher;
	sceneDesc.filterShader = FilterShader;
	sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS | PxSceneFlag::eENABLE_CCD;
	m_scene = m_physics->createScene(sceneDesc);
}


