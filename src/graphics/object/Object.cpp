#include "Object.h"
#include "../../physx/PhysXUtil.h"
#include <algorithm>

Object *Object::SetPos(glm::vec3 p, bool updatePhysics) {
  pos = p;
  if (physActor != nullptr && updatePhysics) {
    physx::PxTransform t = physActor->getGlobalPose();
    t.p = PhysXUtil::glmVec3ToPhys(pos);
    physActor->setGlobalPose(t);
  }
  UpdateModel();
  return this;
}

Object *Object::Translate(glm::vec3 p, bool updatePhysics) {
  pos += p;
  if (physActor != nullptr && updatePhysics) {
    physx::PxTransform t = physActor->getGlobalPose();
    t.p = PhysXUtil::glmVec3ToPhys(pos);
    physActor->setGlobalPose(t);
  }
  UpdateModel();
  return this;
}

Object *Object::SetScale(float s) {
  scale = glm::vec3(s, s, s);

  if (physActor != nullptr) {
    //TODO: Allow more shapes
    physx::PxShape *shape[1]{nullptr};
    physActor->getShapes(shape, 1);
    if (shape[0] != nullptr) {
      physActor->detachShape(*shape[0]);
      // TODO: More than box geometry
      shape[0]->setGeometry(physx::PxBoxGeometry{s, s, s});
      physActor->attachShape(*shape[0]);
    }
  }

  UpdateModel();
  return this;
}

Object *Object::SetScale(glm::vec3 s) {
  scale = s;

  if (physActor != nullptr) {
    //TODO: Allow more shapes
    physx::PxShape *shape[1]{nullptr};
    physActor->getShapes(shape, 1);
    if (shape[0] != nullptr) {
      physActor->detachShape(*shape[0]);
      // TODO: More than box geometry
      shape[0]->setGeometry(physx::PxBoxGeometry{s.x, s.y, s.z});
      physActor->attachShape(*shape[0]);
    }
  }

  UpdateModel();
  return this;
}

Object *Object::SetRot(glm::vec3 r, bool updatePhysics) {
  rot = r;

  if (physActor != nullptr && updatePhysics) {
    physx::PxTransform t = physActor->getGlobalPose();
    t.q = PhysXUtil::glmVec3RotToPhysQuat(rot);
    physActor->setGlobalPose(t);
  }

  UpdateModel();
  return this;
}

Object *Object::SetPhysicsActor(physx::PxRigidActor *actor) {
  actor->userData = this;
  physActor = actor;
  return this;
}

void Object::UpdateModel() {
  // scale
  glm::mat4 scaleM = glm::scale(glm::mat4(1.0), scale);

  // rotate
  glm::mat4 rotationM(1.0);
  rotationM = glm::rotate(rotationM, glm::radians(rot.x), glm::vec3(1.0, 0.0, 0.0));
  rotationM = glm::rotate(rotationM, glm::radians(rot.y), glm::vec3(0.0, 1.0, 0.0));
  rotationM = glm::rotate(rotationM, glm::radians(rot.z), glm::vec3(0.0, 0.0, 1.0));

  // translate to "pos"
  glm::mat4 origin2posM = glm::translate(glm::mat4(1.0), pos);

  // concatenate matrices
  model = origin2posM * rotationM * scaleM;
}

Object *Object::SetTexScale(float t) {
  texScale = t;
  return this;
}


void Object::AddComponent(Component *c) {
  c->object = this;
  components.emplace_back(c);
}

void Object::StartComponents() {
  std::for_each(components.begin(), components.end(), [](Component *c) { c->Start(); });
}

void Object::UpdateComponents() {
  std::for_each(components.begin(), components.end(), [](Component *c) { c->Update(); });
}
