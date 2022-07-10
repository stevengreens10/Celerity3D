#ifndef GUI_OBJECT_H
#define GUI_OBJECT_H

#include "glm/vec3.hpp"
#include "../VertexArray.h"
#include "../IndexBuffer.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Material.h"

class Object {
private:
    glm::vec3 pos{};
    glm::vec3 scale{};
    glm::vec3 rot{};
    glm::mat4 model;
public:
    explicit Object() {
      pos = glm::vec3(0.0f);
      scale = glm::vec3(1.0f);
      rot = glm::vec3(0.0f);
    }

    inline const glm::vec3 &Pos() const { return pos; }

    void SetPos(glm::vec3 p);

    void Translate(glm::vec3 p);

    void SetScale(float s);

    void SetScale(glm::vec3 s);

    inline const glm::vec3 &Scale() const { return scale; }

    inline const glm::vec3 &Rot() const { return rot; }

    void SetRot(glm::vec3 r);

    inline const glm::mat4 &Model() const { return model; }

    void UpdateModel();

    virtual void Draw() = 0;

};


#endif //GUI_OBJECT_H
