#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <fstream>
#include <string>
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include "glm/detail/type_mat4x4.hpp"

using std::vector, std::string;

#pragma pack(push, 1)
typedef struct {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
} Vertex;

typedef struct {
    glm::vec2 pos;
    glm::vec2 uv;
} Vertex2D;

struct TransformationData {
    glm::mat4 vp;
    glm::mat4 model;
};
#pragma pack(pop)

char *readFile(const string &path);

// for string delimiter
vector<string> split(const string &s, const string &delimiter);

glm::vec4 color(float r, float g, float b, float a);

glm::vec4 color(int r, int g, int b, int a);

glm::vec2 sToVec2(vector<string> s, int startIdx);

glm::vec3 sToVec3(vector<string> s, int startIdx);

void calculateTangents(vector<Vertex> &vertices, const vector<uint32_t> &indices);

#endif //GUI_UTIL_H
