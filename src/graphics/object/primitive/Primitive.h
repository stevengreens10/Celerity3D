#ifndef GUI_PRIMITIVE_H
#define GUI_PRIMITIVE_H

#include "../Object.h"

class Primitive : public Object {
public:
    std::reference_wrapper<Material> material;

    explicit Primitive(std::reference_wrapper<Material> material) : material(material) {}

    void Draw(Shader &shader) override {
      getVertexBuffer()->Bind();
      material.get().Bind(shader);
      glDrawArrays(GL_TRIANGLES, 0, getNumVertices());
    }

private:
    virtual VertexArray *getVertexBuffer() = 0;

    virtual int getNumVertices() = 0;
};

#endif //GUI_PRIMITIVE_H
