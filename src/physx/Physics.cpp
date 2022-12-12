//
// Created by steve on 12/11/2022.
//

#include "Physics.h"
#include "omnipvd/PxOmniPvd.h"
#include "pvdruntime/OmniPvdWriter.h"
#include "pvdruntime/OmniPvdFileWriteStream.h"
#include "PhysXUtil.h"

void Physics::init() {
  // INIT PHYSX
  physx::PxAllocatorCallback *allocatorCallback = new physx::PxDefaultAllocator();
  physx::PxErrorCallback *errorCallback = new physx::PxDefaultErrorCallback();
  pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocatorCallback, *errorCallback);
  if (!pxFoundation)
    Log::fatalf("PxCreateFoundation failed!");

  // Create the OmniPVD instance
  omniPvd = PxCreateOmniPvd(*pxFoundation);

  if (!omniPvd) {
    Log::fatalf("Could not set up omnipvd");
  }
  OmniPvdWriter *omniWriter = omniPvd->getWriter();
  // Uncomment for debugging the OmniPvd write stream
  omniWriter->setLogFunction([](char *logMsg) {std::cout << logMsg << std::endl;});
  OmniPvdFileWriteStream *omniFileWriteStream = omniPvd->getFileWriteStream();
  if (omniWriter && omniFileWriteStream) {
    omniWriter->setWriteStream((OmniPvdWriteStream *) omniFileWriteStream);
  }

  physics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation,
                            physx::PxTolerancesScale(100, 981), true, nullptr, omniPvd);
  if (!physics)
    Log::fatalf("PxCreatePhysics failed!");

  // Once the PhysX instance is created with a non-zero OmniPVD object, one can now proced to set the simulation
  omniFileWriteStream->setFileName("myoutputfile.ovd");
  omniPvd->startSampling();

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

physx::PxActor **Physics::simulate(float deltaT, int &numActors) {
  pxScene->simulate(physx::PxReal(deltaT));
  /* blocking */
  pxScene->fetchResults(true);

  physx::PxU32 pxNumActors;
  physx::PxActor **actors = pxScene->getActiveActors(pxNumActors);
  numActors = pxNumActors;
  Log::logf("Simulating %f seconds. Updating %d physics actors", deltaT, numActors);
  return actors;
}


physx::PxRigidStatic *Physics::createRigidStatic(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, PhysicsMaterial mat) {
  physx::PxMaterial *physMaterial = physics->createMaterial(mat.staticFriction, mat.dynamicFriction, mat.restitution);
  physx::PxTransform transform = PhysXUtil::transformFromPosRot(pos, rot);
  // TODO: Not just cubes
  physx::PxShape *shape = physics->createShape(
          physx::PxBoxGeometry(scale.x, scale.y, scale.z), *physMaterial);
  physx::PxRigidStatic *rigidStatic = physics->createRigidStatic(transform);
  rigidStatic->attachShape(*shape);
  return rigidStatic;
}

physx::PxRigidDynamic *Physics::createRigidDynamic(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, PhysicsMaterial mat) {
  physx::PxMaterial *physMaterial = physics->createMaterial(mat.staticFriction, mat.dynamicFriction, mat.restitution);
  physx::PxTransform transform = PhysXUtil::transformFromPosRot(pos, rot);
  // TODO: Not just cubes
  physx::PxShape *shape = physics->createShape(
          physx::PxBoxGeometry(scale.x, scale.y, scale.z), *physMaterial);
  physx::PxRigidDynamic *rigidDynamic = physics->createRigidDynamic(transform);
  rigidDynamic->attachShape(*shape);
  return rigidDynamic;
}

void Physics::addActor(physx::PxActor *actor) {
  pxScene->addActor(*actor);
}

void Physics::cleanup() {
  PX_RELEASE(pxScene);
  PX_RELEASE(physics);
  omniPvd->release();
  PX_RELEASE(pxFoundation);
}