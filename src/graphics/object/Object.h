#ifndef GUI_OBJECT_H
#define GUI_OBJECT_H

#include "glm/vec3.hpp"
#include "../VertexArray.h"
#include "../IndexBuffer.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Material.h"

class Object {
public:
    glm::vec3 pos{};
    float scale{};
    glm::vec3 rot{};

    explicit Object() {
      pos = glm::vec3(0.0f);
      scale = 1.0f;
      rot = glm::vec3(0.0f);
    }

    virtual void Draw() = 0;

};


#endif //GUI_OBJECT_H
