#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <fstream>

typedef struct {
    float f1, f2;
} Float2;

typedef struct {
    float f1, f2, f3;
} Float3;

typedef struct {
    Float3 pos;
    Float2 uv;
} Vertex;

char *readFile(const char *path);

#endif //GUI_UTIL_H
