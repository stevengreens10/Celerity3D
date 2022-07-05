#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <fstream>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

typedef struct {
    glm::vec3 pos;
    glm::vec2 uv;
} Vertex;

char *readFile(const char *path);

#endif //GUI_UTIL_H
