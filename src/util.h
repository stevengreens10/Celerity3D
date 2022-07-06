#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <fstream>
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

using std::vector, std::string;

typedef struct {
    glm::vec3 pos;
    glm::vec2 uv;
//    glm::vec3 normal;
} Vertex;

char *readFile(const string &path);

// for string delimiter
vector<string> split(const string &s, const string &delimiter);

#endif //GUI_UTIL_H
