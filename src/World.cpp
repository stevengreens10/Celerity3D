#include "World.h"
#include "graphics/Renderer.h"
#include "physx/PhysXUtil.h"
#include "physx/Physics.h"
#include <algorithm>

World::~World() {
  // TODO: Clean up lights and objects

}

void World::Update(float deltaT) {
  int numActors;
  physx::PxActor **activeActors = Physics::simulate(deltaT, numActors);

  for (int i = 0; i < numActors; i++) {
    physx::PxActor *activeActor = activeActors[i];
    auto *worldObj = (Object *) activeActor->userData;
    if (!worldObj) {
      Log::fatalf("Could not retrieve world object from physX actor!");
    }

    physx::PxTransform transform = ((physx::PxRigidActor *) activeActor)->getGlobalPose();

    auto [pos, rot] = PhysXUtil::posAndRotFromTransform(&transform);

    worldObj->SetPos(pos);
    worldObj->SetRot(rot);
  }

  std::for_each(objects.begin(), objects.end(), [](Object *o) { o->UpdateComponents(); });
}


void World::AddObject(Object *object) {
  objects.push_back(object);
  auto actor = object->PhysicsActor();
  if (actor)
    Physics::addActor(actor);
  object->StartComponents();
}

void World::RemoveObject(Object *object) {
  auto toRemove = std::remove(objects.begin(), objects.end(), object);
  objects.erase(toRemove, objects.end());
}

void World::AddLight(LightSource *light) {
  if (lights.size() < MAX_LIGHTS)
    lights.push_back(light);
}



