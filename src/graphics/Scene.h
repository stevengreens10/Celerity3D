#ifndef GUI_SCENE_H
#define GUI_SCENE_H

#include <vector>
#include "LightSource.h"
#include "object/Object.h"

#pragma pack(push, 1)
struct SceneData {
    glm::vec3 camPos;
    std::vector<LightSource> lightSources;
};
#pragma pack(pop)

class Scene {
private:
    std::vector<LightSource *> lights;
    std::vector<Object *> objects;
public:

    ~Scene();

    void AddObject(Object *object);

    void RemoveObject(Object *object);

    void AddLight(LightSource *light);

    inline const vector<LightSource *> &Lights() const { return lights; }

    inline const vector<Object *> &Objects() const { return objects; }
};


#endif //GUI_SCENE_H
