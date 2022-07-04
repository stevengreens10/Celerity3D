#ifndef GUI_RENDERABLE_H
#define GUI_RENDERABLE_H

#include "glm/vec3.hpp"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

class Renderable {
public:
    glm::vec3 pos;
    float scale;
    glm::vec3 rot;
    VertexArray *vao;
    IndexBuffer *ibo;
    Shader *shader;
    Texture *texture;
};


#endif //GUI_RENDERABLE_H
