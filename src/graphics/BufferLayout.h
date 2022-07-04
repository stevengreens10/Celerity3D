#ifndef GUI_BUFFERLAYOUT_H
#define GUI_BUFFERLAYOUT_H

#include <vector>
#include "GL/glew.h"

struct VertexBufferElement {
  unsigned int type;
  unsigned int count;
  unsigned int offset;
  bool normalized;
};

class BufferLayout {

private:
    std::vector<VertexBufferElement> elements;
    unsigned int stride;
public:

    BufferLayout() : stride(0) {}

    template <typename T>
    void Push(unsigned int count);

    inline const std::vector<VertexBufferElement>& GetElements() const { return elements; }
    inline unsigned int GetStride() const { return stride; }
};


#endif //GUI_BUFFERLAYOUT_H
