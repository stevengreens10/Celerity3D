#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#ifndef GUI_SQUARE_H
#define GUI_SQUARE_H

#include <memory>
#include "../Object.h"
#include "Primitive.h"

class Square : public Primitive {
private:
    inline static std::shared_ptr<VertexArray> vao;
    inline static int numVertices;
public:
    explicit Square(std::reference_wrapper<Material> m) : Primitive(m) {}

    static void InitBuffers();

    void Draw(Shader &shader) override;

private:
    static std::shared_ptr<VertexArray> CreateVertexBuffer();

    inline VertexArray *getVertexBuffer() override {
      return vao.get();
    }

    inline int getNumVertices() override {
      return numVertices;
    }
};


#endif //GUI_SQUARE_H

#pragma clang diagnostic pop