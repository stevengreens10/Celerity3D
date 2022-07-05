#ifndef GUI_TRIANGULARPRISM_H
#define GUI_TRIANGULARPRISM_H

#include <memory>
#include "Renderable.h"

class TriangularPrism : public Renderable {
private:
    inline static std::shared_ptr<VertexArray> VAO;
    inline static std::shared_ptr<IndexBuffer> IBO;
public:
    explicit TriangularPrism(Material &material);

    static void InitBuffers();

private:
    static std::shared_ptr<VertexArray> CreateVertexBuffer();

    static std::shared_ptr<IndexBuffer> CreateIndexBuffer();
};

#endif //GUI_TRIANGULARPRISM_H
