#ifndef GUI_LIGHTSOURCE_H
#define GUI_LIGHTSOURCE_H

#include "glm/vec3.hpp"

typedef int LightType;
const LightType LIGHT_POINT = 0;
const LightType LIGHT_DIR = 1;
const LightType LIGHT_SPOT = 2;

struct __attribute__ ((packed)) LightSource {
    LightType type;
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 color;
    glm::vec3 intensities;
    glm::vec3 attenuation = glm::vec3(1.0f, 0.015f, 0.001f);
};

#endif //GUI_LIGHTSOURCE_H
