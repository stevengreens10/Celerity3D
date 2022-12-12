#ifndef GUI_OBJECT_H
#define GUI_OBJECT_H

#include "glm/vec3.hpp"
#include "../VertexArray.h"
#include "../IndexBuffer.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Material.h"
#include "PxPhysicsAPI.h"
#include "../../Component.h"

// Forward declaration
class Component;

class Object {
private:
    glm::vec3 pos{};
    glm::vec3 scale{};
    // In degrees
    glm::vec3 rot{};
    glm::mat4 model;

    physx::PxRigidActor *physActor = nullptr;

    float texScale = 1.0f;

    std::vector<Component *> components;
public:
    bool useLighting = true;


    explicit Object() {
      pos = glm::vec3(0.0f);
      scale = glm::vec3(1.0f);
      rot = glm::vec3(0.0f);
    }

    inline const glm::vec3 &Pos() const { return pos; }

    Object *SetPos(glm::vec3 p);

    Object *SetRot(glm::vec3 r);

    Object *Translate(glm::vec3 p);

    Object *SetScale(float s);

    Object *SetScale(glm::vec3 s);

    Object *SetPhysicsActor(physx::PxRigidActor *actor);

    inline const glm::vec3 &Scale() const { return scale; }

    inline const glm::vec3 &Rot() const { return rot; }

    inline physx::PxActor *PhysicsActor() { return physActor; }

    inline float TexScale() const { return texScale; }

    Object *SetTexScale(float texScale);

    inline const glm::mat4 &Model() const { return model; }

    void UpdateModel();

    void AddComponent(Component *c);

    void StartComponents();

    void UpdateComponents();

    virtual void Draw(Shader &shader) = 0;

};


#endif //GUI_OBJECT_H
