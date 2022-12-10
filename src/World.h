#ifndef GUI_WORLD_H
#define GUI_WORLD_H

#include <vector>
#include "graphics/LightSource.h"
#include "graphics/object/Object.h"
#include "PxPhysicsAPI.h"

#pragma pack(push, 1)
struct SceneData {
    glm::vec3 camPos;
    std::vector<LightSource> lightSources;
};
#pragma pack(pop)

class World {
private:
    std::vector<LightSource *> lights;
    std::vector<Object *> objects;

    physx::PxFoundation *pxFoundation;
    physx::PxScene *pxScene;

    std::unordered_map<physx::PxActor *, Object *> pxActorMap;
public:
    physx::PxPhysics *physics;
    Texture *skyboxTex;

    ~World();

    void InitPhysics();

    void SimulatePhysics(float deltaT);

    void AddObject(physx::PxActor *actor, Object *object);

    void RemoveObject(Object *object);

    void AddLight(LightSource *light);

    inline const vector<LightSource *> &Lights() const { return lights; }

    inline const vector<Object *> &Objects() const { return objects; }
};


#endif //GUI_WORLD_H
