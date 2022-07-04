#ifndef GUI_VERTEXARRAY_H
#define GUI_VERTEXARRAY_H

#include <vector>
#include "VertexBuffer.h"
#include "BufferLayout.h"

class VertexArray {
private:
    unsigned int rendererID;
    std::vector<unsigned int> bufIds;
public:
    VertexArray();

    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void AddBuffer(const VertexBuffer &buffer, const BufferLayout &layout);
};


#endif //GUI_VERTEXARRAY_H
