#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#ifndef GUI_CUBE_H
#define GUI_CUBE_H

#include <memory>
#include "Renderable.h"

class Cube : public Renderable {
private:
    inline static std::shared_ptr<VertexArray> CUBE_VAO;
    inline static std::shared_ptr<IndexBuffer> CUBE_IBO;
public:
    Cube(Material &material);

    static void InitBuffers();

private:
    static std::shared_ptr<VertexArray> CreateVertexBuffer();

    static std::shared_ptr<IndexBuffer> CreateIndexBuffer();
};


#endif //GUI_CUBE_H

#pragma clang diagnostic pop