#ifndef GUI_SCENE_H
#define GUI_SCENE_H

#include <vector>
#include "LightSource.h"
#include "Renderable.h"

struct __attribute__ ((packed)) SceneData {
    glm::vec3 camPos;
    std::vector<LightSource> lightSources;
};

class Scene {
private:
    std::vector<LightSource *> lights;
    std::vector<Renderable *> objects;
public:
    void AddObject(Renderable *object);

    void RemoveObject(Renderable *object);

    void AddLight(LightSource *light);

    inline const vector<LightSource *> &Lights() const { return lights; }

    inline const vector<Renderable *> &Objects() const { return objects; }
};


#endif //GUI_SCENE_H
