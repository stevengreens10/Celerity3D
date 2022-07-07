#ifndef GUI_BUFFERLAYOUT_H
#define GUI_BUFFERLAYOUT_H

#include <vector>
#include "GL/glew.h"
#include "../log.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <cstdio>

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned int size;
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
        Log::logf("Invalid type for BufferLayout push: ", (typeid(T)).name());
      }
      auto glType = glTypeMap[type];
      elements.push_back({glType, count, sizeof(T), stride, false});
      stride += sizeof(T) * count;
    }

    [[nodiscard]] inline const std::vector<VertexBufferElement> &GetElements() const { return elements; }

    [[nodiscard]] inline unsigned int GetStride() const { return stride; }

    static unsigned int GetBaseAlignment(unsigned int type) {
      switch (type) {
        case GL_FLOAT:
        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_BYTE:
          return 4;
        case GL_FLOAT_VEC2:
          return 8;
        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC4:
        case GL_FLOAT_MAT4:
          return 16;
        default:
          return 4;
      }
    }

private:
    static std::unordered_map<std::type_index, unsigned int> createMap() {
      std::unordered_map<std::type_index, unsigned int> m;
      m[typeid(float)] = GL_FLOAT;
      m[typeid(glm::vec2)] = GL_FLOAT_VEC2;
      m[typeid(glm::vec3)] = GL_FLOAT_VEC3;
      m[typeid(glm::vec4)] = GL_FLOAT_VEC4;
      m[typeid(glm::mat4)] = GL_FLOAT_MAT4;
      m[typeid(unsigned int)] = GL_UNSIGNED_INT;
      m[typeid(unsigned char)] = GL_UNSIGNED_BYTE;
      return m;
    }
};

#endif //GUI_BUFFERLAYOUT_H
