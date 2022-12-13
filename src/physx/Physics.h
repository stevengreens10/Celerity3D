#ifndef GUI_PHYSICS_H
#define GUI_PHYSICS_H


#include "PxPhysicsAPI.h"
#include "../log.h"
#include "glm/vec3.hpp"
#include "PhysicsMaterial.h"

class Physics {
private:
    static inline physx::PxFoundation *pxFoundation;
    static inline physx::PxOmniPvd *omniPvd;
    static inline physx::PxScene *pxScene;
    static inline physx::PxPhysics *physics;
public:

    static void init();

    static std::vector<physx::PxActor *> simulate(float deltaT, int &numActors);

    static physx::PxRigidStatic *createRigidStatic(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, PhysicsMaterial mat);

    static physx::PxRigidDynamic *
    createRigidDynamic(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, PhysicsMaterial mat);


    static void addActor(physx::PxActor *actor);

    static void cleanup();
};


#endif //GUI_PHYSICS_H
