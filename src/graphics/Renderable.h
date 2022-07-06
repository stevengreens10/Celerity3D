#ifndef GUI_RENDERABLE_H
#define GUI_RENDERABLE_H

#include "glm/vec3.hpp"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

class Renderable {
public:
    glm::vec3 pos{};
    float scale{};
    glm::vec3 rot{};
    VertexArray *vao{};
    IndexBuffer *ibo{};
    std::reference_wrapper<Material> material;

    explicit Renderable(std::reference_wrapper<Material> material) : material(material) {
      pos = glm::vec3(0.0f);
      scale = 1.0f;
      rot = glm::vec3(0.0f);
    }

    ~Renderable() {
      delete vao;
      delete ibo;
    }

    virtual void Draw() const {
      vao->Bind();
      ibo->Bind();
      material.get().Bind();
      glDrawElements(GL_TRIANGLES, (int) ibo->getCount(), GL_UNSIGNED_INT, nullptr);
    }
};


#endif //GUI_RENDERABLE_H
