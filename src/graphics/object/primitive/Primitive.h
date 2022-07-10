#ifndef GUI_PRIMITIVE_H
#define GUI_PRIMITIVE_H

#include "../Object.h"

class Primitive : public Object {
public:
    std::reference_wrapper<Material> material;

    explicit Primitive(std::reference_wrapper<Material> material) : material(material) {}

    virtual void Draw() override {
      getVertexBuffer()->Bind();
      getIndexBuffer()->Bind();
      material.get().Bind();
      glDrawElements(GL_TRIANGLES, (int) getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
    }

private:
    virtual VertexArray *getVertexBuffer() = 0;

    virtual IndexBuffer *getIndexBuffer() = 0;
};

#endif //GUI_PRIMITIVE_H
