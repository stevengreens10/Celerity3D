#ifndef GUI_BUFFERLAYOUT_H
#define GUI_BUFFERLAYOUT_H

#include <vector>
#include "GL/glew.h"
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <cstdio>

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
    static std::unordered_map<std::type_index, unsigned int> glTypeMap;

    BufferLayout() : stride(0) {}

    template<typename T>
    void Push(unsigned int count) {
      const std::type_index &type = std::type_index(typeid(T));
      if (!glTypeMap.contains(type)) {
        printf("Invalid type for BufferLayout push: \n", (typeid(T)).name());
      }
      auto glType = glTypeMap[type];
      elements.push_back({glType, count, stride, false});
      stride += sizeof(T) * count;
    }

    inline const std::vector<VertexBufferElement> &GetElements() const { return elements; }

    inline unsigned int GetStride() const { return stride; }

private:
    static std::unordered_map<std::type_index, unsigned int> createMap() {
      std::unordered_map<std::type_index, unsigned int> m;
      m[typeid(float)] = GL_FLOAT;
      m[typeid(unsigned int)] = GL_UNSIGNED_INT;
      m[typeid(unsigned char)] = GL_UNSIGNED_BYTE;
      return m;
    }
};

#endif //GUI_BUFFERLAYOUT_H
