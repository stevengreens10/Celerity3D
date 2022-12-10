#include "World.h"
#include "graphics/Renderer.h"
#include "physx/PhysXUtil.h"
#include <algorithm>

World::~World() {
  // TODO: Clean up lights and objects

  // Clean up physics
  PX_RELEASE(pxScene);
  PX_RELEASE(physics);
  PX_RELEASE(pxFoundation);
}

void World::InitPhysics() {
  // INIT PHYSX
  physx::PxAllocatorCallback *allocatorCallback = new physx::PxDefaultAllocator();
  physx::PxErrorCallback *errorCallback = new physx::PxDefaultErrorCallback();
  pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocatorCallback, *errorCallback);
  if (!pxFoundation)
    Log::fatalf("PxCreateFoundation failed!");

  physics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation,
                            physx::PxTolerancesScale(100, 981), false);
  if (!physics)
    Log::fatalf("PxCreatePhysics failed!");
  physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
  sceneDesc.gravity = physx::PxVec3(0, -9.81f, 0);
//    sceneDesc.bounceThresholdVelocity = physx::PxReal(?f);
  physx::PxDefaultCpuDispatcher *dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
  sceneDesc.cpuDispatcher = dispatcher;
  sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
  pxScene = physics->createScene(sceneDesc);
  pxScene->setFlag(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);

  Log::logf("PhysX initialized!");
}

void World::SimulatePhysics(float deltaT) {
  pxScene->simulate(physx::PxReal(deltaT));
  /* blocking */
  pxScene->fetchResults(true);

  physx::PxU32 numActors;
  physx::PxActor **activeActors = pxScene->getActiveActors(numActors);
  Log::logf("Simulating %f seconds. Updating %d physics actors", deltaT, numActors);
  for (int i = 0; i < numActors; i++) {
    physx::PxActor *activeActor = activeActors[i];
    Object *worldObj = pxActorMap[activeActor];
    if (!worldObj) {
      Log::fatalf("Could not retrieve world object from physX actor!");
    }

    physx::PxTransform transform = ((physx::PxRigidActor *) activeActor)->getGlobalPose();

    auto [pos, rot] = PhysXUtil::posAndRotFromTransform(&transform);

    worldObj->SetPos(pos);
    worldObj->SetRot(rot);
  }
}


void World::AddObject(physx::PxActor *actor, Object *object) {
  objects.push_back(object);
  if (actor != nullptr) {
    pxScene->addActor(*actor);
    pxActorMap[actor] = object;
  }
}

void World::RemoveObject(Object *object) {

  for (auto const &[actor, obj]: pxActorMap) {
    if (obj == object && actor != nullptr) {
      pxActorMap.erase(actor);
      break;
    }
  }

  auto toRemove = std::remove(objects.begin(), objects.end(), object);
  objects.erase(toRemove, objects.end());
}

void World::AddLight(LightSource *light) {
  if (lights.size() < MAX_LIGHTS)
    lights.push_back(light);
}



