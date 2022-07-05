#ifndef GUI_VERTEXARRAY_H
#define GUI_VERTEXARRAY_H

#include <vector>
#include "VertexBuffer.h"
#include "BufferLayout.h"

class VertexArray {
private:
    unsigned int rendererID = 0;
    std::vector<unsigned int> bufIds;
public:
    VertexArray();

    ~VertexArray();

    void Bind() const;

    static void Unbind();

    void AddBuffer(const VertexBuffer &buffer, const BufferLayout &layout);
};


#endif //GUI_VERTEXARRAY_H
